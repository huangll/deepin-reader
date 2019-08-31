#include "MsgSubject.h"
#include "MsgHeader.h"

MsgSubject::MsgSubject()
{

}

void MsgSubject::sendMsg(const int &msgType, const QString &msgContent)
{
    Notify(msgType, msgContent);
}

void MsgSubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void MsgSubject::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void MsgSubject::Notify(const int &msgType, const QString &msgContent)
{
    foreach(IObserver* obs, m_observerList)
    {
        /*  如果 该消息 在某一个 观察者中被处理了， 就返回0， 若没有处理， 则继续传递给下一个观察者，
        ！！！消息截断！！！
        */
        int nRes = obs->update(msgType, msgContent);
        if(nRes == ConstantMsg::g_effective_res)
        {
            break;
        }
    }
}
