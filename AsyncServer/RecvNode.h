//
// Created by AdminCZ on 2026/4/10.
//

#ifndef ASYNCSERVER_RECVNODE_H
#define ASYNCSERVER_RECVNODE_H
#include "MsgNode.h"

class RecvNode : public MsgNode
{
public:
    explicit  RecvNode( MSG_LEN_TYPE len, MSG_ID_TYPE id = INVALID_MSG_ID);
    ~RecvNode() override;
    MSG_ID_TYPE Get_Msg_Id() const;
private:
    MSG_ID_TYPE m_msg_id;
};



#endif //ASYNCSERVER_RECVNODE_H
