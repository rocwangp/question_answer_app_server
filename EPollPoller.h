#pragma once

#include "Poller.h"

struct epoll_event;

class EPollPoller : public Poller
{
public:
    EPollPoller();
    virtual ~EPollPoller();

    virtual void poll(ChannelList& activeChannels);
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);
    virtual void update(int opt, Channel* channel);
private:
    void fillActiveQueue(ChannelList& activeChannels, int numEvents);
private:

    typedef std::vector<struct epoll_event> EventList;

    int epollfd_;
    EventList events_;

    static int kEPollEventSize;
};
