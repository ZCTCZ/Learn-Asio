//
// Created by AdminCZ on 2026/4/10.
//

#ifndef ASYNCSERVER_SENDNODE_H
#define ASYNCSERVER_SENDNODE_H
#include "MsgNode.h"

class SendNode : public MsgNode
{
public:
    explicit SendNode(const char *msg, MSG_LEN_TYPE len, MSG_ID_TYPE id);
    ~SendNode() override;
    MSG_ID_TYPE Get_Msg_Id() const;
private:
    MSG_ID_TYPE m_msg_id;
};



#endif //ASYNCSERVER_SENDNODE_H
