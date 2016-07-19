//--------------------------------------------------------------------------
// Copyright (C) 2014-2016 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------
// nhttp_transaction.cc author Tom Peters <thopeter@cisco.com>

#include <sys/types.h>

#include "nhttp_enum.h"
#include "nhttp_transaction.h"
#include "nhttp_msg_request.h"
#include "nhttp_msg_status.h"
#include "nhttp_msg_header.h"
#include "nhttp_msg_trailer.h"
#include "nhttp_msg_body.h"

using namespace NHttpEnums;

NHttpTransaction::~NHttpTransaction()
{
    delete request;
    delete status;
    delete header[SRC_CLIENT];
    delete header[SRC_SERVER];
    delete trailer[SRC_CLIENT];
    delete trailer[SRC_SERVER];
    delete latest_body;
}

NHttpTransaction* NHttpTransaction::attach_my_transaction(NHttpFlowData* session_data, SourceId
    source_id)
{
    // This factory method:
    // 1. creates new transactions for all request messages and orphaned response messages
    // 2. associates requests and responses and supports pipelining
    // 3. garbage collects unneeded transactions
    // 4. returns the current transaction

    // Request section: replace the old request transaction with a new transaction.
    if (session_data->section_type[source_id] == SEC_REQUEST)
    {
        // If the HTTP request and response messages are alternating (usual situation) the old
        // request transaction will have been moved to the server side when the last response
        // message was received. This will be nullptr and we don't need to deal with the old
        // request transaction here.
        if (session_data->transaction[SRC_CLIENT] != nullptr)
        {
            // The old request transaction is still here. Typically that is because the
            // the current request has arrived before the previous response (pipelining). We need
            // to add this transaction to our pipeline where it will wait for the matching
            // response. But there are some special cases to check first.
            if (session_data->transaction[SRC_CLIENT]->response_seen)
            {
                // The response started before the request finished. When the response took the
                // old request transaction it did not leave the usual nullptr because we still
                // needed it. Instead the two sides have been sharing the transaction. This is a
                // soft delete that eliminates our interest in this transaction without disturbing
                // the possibly ongoing response processing.
                delete_transaction(session_data->transaction[SRC_CLIENT]);
            }
            else if ((session_data->pipeline_overflow) || (session_data->pipeline_underflow))
            {
                // Pipelining previously broke down and both sides are processed separately from
                // now on. We just throw things away when we are done with them.
                delete_transaction(session_data->transaction[SRC_CLIENT]);
            }
            else if (!session_data->add_to_pipeline(session_data->transaction[SRC_CLIENT]))
            {
                // The pipeline is full and just overflowed.
                session_data->infractions[source_id] += INF_PIPELINE_OVERFLOW;
                session_data->events[source_id].create_event(EVENT_PIPELINE_MAX);
                delete_transaction(session_data->transaction[SRC_CLIENT]);
            }
        }
        session_data->transaction[SRC_CLIENT] = new NHttpTransaction;
    }
    // This transaction has more than one response. This is a new response which is replacing the
    // interim response. The two responses cannot coexist so we must clean up the interim response.
    else if ((session_data->section_type[source_id] == SEC_STATUS) &&
             (session_data->transaction[SRC_SERVER] != nullptr) &&
              session_data->transaction[SRC_SERVER]->second_response_expected)
    {
        assert(session_data->transaction[SRC_SERVER] != nullptr);
        session_data->transaction[SRC_SERVER]->second_response_expected = false;
        delete session_data->transaction[SRC_SERVER]->status;
        session_data->transaction[SRC_SERVER]->status = nullptr;
        delete session_data->transaction[SRC_SERVER]->header[SRC_SERVER];
        session_data->transaction[SRC_SERVER]->header[SRC_SERVER] = nullptr;
        delete session_data->transaction[SRC_SERVER]->trailer[SRC_SERVER];
        session_data->transaction[SRC_SERVER]->trailer[SRC_SERVER] = nullptr;
    }
    // Status section: delete the current transaction and get a new one from the pipeline. If the
    // pipeline is empty check for a request transaction and take it. If there is no transaction
    // available then declare an underflow and create a new transaction specifically for the
    // response side.
    else if (session_data->section_type[source_id] == SEC_STATUS)
    {
        delete_transaction(session_data->transaction[SRC_SERVER]);
        if (session_data->pipeline_underflow)
        {
            // A previous underflow separated the two sides forever
            session_data->transaction[SRC_SERVER] = new NHttpTransaction;
        }
        else if ((session_data->transaction[SRC_SERVER] = session_data->take_from_pipeline()) ==
            nullptr)
        {
            if ((session_data->transaction[SRC_CLIENT] == nullptr) ||
                (session_data->transaction[SRC_CLIENT]->response_seen))
            {
                // Either there is no request at all or there is a request but a previous response
                // already took it. Either way we have more responses than requests.
                session_data->pipeline_underflow = true;
                session_data->transaction[SRC_SERVER] = new NHttpTransaction;
            }

            else if (session_data->type_expected[SRC_CLIENT] == SEC_REQUEST)
            {
                // This is the normal case where the requests and responses are alternating (no
                // pipelining). Processing of the response is complete so the request just takes
                // it.
                session_data->transaction[SRC_SERVER] = session_data->transaction[SRC_CLIENT];
                session_data->transaction[SRC_CLIENT] = nullptr;
            }
            else
            {
                // Response message is starting before the request message has finished. Request
                // side is not finished with this transaction so two sides share it
                session_data->transaction[SRC_CLIENT]->shared_ownership = true;
                session_data->transaction[SRC_SERVER] = session_data->transaction[SRC_CLIENT];
            }
        }
        session_data->transaction[SRC_SERVER]->response_seen = true;
    }

    assert(session_data->transaction[source_id] != nullptr);
    return session_data->transaction[source_id];
}

void NHttpTransaction::delete_transaction(NHttpTransaction* transaction)
{
    if (transaction != nullptr)
    {
        if (!transaction->shared_ownership)
            delete transaction;
        else
            transaction->shared_ownership = false;
    }
}

void NHttpTransaction::set_body(NHttpMsgBody* latest_body_)
{
    delete latest_body;
    latest_body = latest_body_;
}

