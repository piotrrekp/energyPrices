#ifndef _DAYSAVER_H_
#define _DAYSAVER_H_

#include "fetchedDayDTO.h"

class daySaver {
public:
	virtual ~daySaver() = default;
	virtual void store(const fetchedDay &) = 0;
};

#endif // _DAYSAVER_H_
