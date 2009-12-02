// Event header file

#ifndef _EVENT_H
#define _EVENT_H

typedef enum event_type
{
	EVENT_OTHER = 0,
	EVENT_DRAW,
	EVENT_CLOSE
} event_type;

// A vanilla event. Cast to the correct type of event according to 'type'.
typedef struct d_event
{
	event_type type;
} d_event;

int event_init();
void event_poll();

// Not to be confused with event_poll, which will be removed eventually, this one sends events to event handlers
void event_process();

#endif
