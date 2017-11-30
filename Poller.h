#pragma once

#include "noncopyable.h"

#include <map>
#include <vector>

class Timestamp;
class Channel;


class Poller : private noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;
    Poller() {}
    virtual ~Poller() {}

    virtual void poll(ChannelList& activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual void update(int opt, Channel* channel) = 0;

protected:
    typedef std::map<int, Channel*> ChannelMap;

    ChannelMap channels_;

};
