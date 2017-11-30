#include "EPollPoller.h"
#include "Channel.h"
#include "Logging.h"

#include <sys/epoll.h>
#include <string.h> //bzero


static const int kNew = -1;
static const int kAdd = 0;
static const int kDelete = 1;

int EPollPoller::kEPollEventSize = 1000;

EPollPoller::EPollPoller()
    : ::Poller(),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kEPollEventSize)
{

}

EPollPoller::~EPollPoller()
{

}


void EPollPoller::poll(ChannelList& activeChannels)
{
    int n = ::epoll_wait(epollfd_, &*events_.begin(), kEPollEventSize, -1);
    if(n > 0)
    {
        fillActiveQueue(activeChannels, n);
    }

}


void EPollPoller::fillActiveQueue(ChannelList& activeChannels, int numEvents)
{
    activeChannels.clear();
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels.push_back(channel);
    }
}


void EPollPoller::updateChannel(Channel* channel)
{
   int state = channel->state();
   int fd = channel->fd();
   if(state == kNew || state == kDelete)
   {
       if(state == kNew)
       {
           channels_[fd] = channel;
       }

       channel->set_state(kAdd);
       update(EPOLL_CTL_ADD, channel);
   }
   else
   {
       if(channel->isNoneEvent())
       {
           update(EPOLL_CTL_DEL, channel);
           channel->set_state(kDelete);
           channels_.erase(channel->fd());
       }
       else
       {
           update(EPOLL_CTL_MOD, channel);
       }
   }
    
}


void EPollPoller::update(int opt, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.ptr = channel;
    event.events = channel->events();

    if(::epoll_ctl(epollfd_, opt, channel->fd(), &event) < 0)
    {
        LOG_ERROR << "epoll_ctl error";
    }
}

void EPollPoller::removeChannel(Channel* channel)
{

}
