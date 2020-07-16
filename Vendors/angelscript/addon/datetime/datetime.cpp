#include "datetime.h"
#include <string.h>
#include <assert.h>
#include <new>

using namespace std;
using namespace std::chrono;

BEGIN_AS_NAMESPACE

// TODO: Allow setting the timezone to use

static tm time_point_to_tm(const std::chrono::time_point<std::chrono::system_clock> &tp)
{
	time_t t = system_clock::to_time_t(tp);
	tm local;
#ifdef _MSC_VER
	localtime_s(&local, &t);
#else
	local = *localtime(&t);
#endif
	return local;
}

// Returns true if successful. Doesn't modify tp if not successful
static bool tm_to_time_point(const tm &_tm, std::chrono::time_point<std::chrono::system_clock> &tp)
{
	tm localTm = _tm;
	localTm.tm_isdst = -1; // Always use current settings, so mktime doesn't modify the time for daylight savings

	time_t t = mktime(&localTm);
	if (t == -1)
		return false;

	// Verify if the members were modified, indicating an out-of-range value in input
	if (localTm.tm_year != _tm.tm_year ||
		localTm.tm_mon != _tm.tm_mon ||
		localTm.tm_mday != _tm.tm_mday ||
		localTm.tm_hour != _tm.tm_hour ||
		localTm.tm_min != _tm.tm_min ||
		localTm.tm_sec != _tm.tm_sec)
		return false;

	tp = system_clock::from_time_t(t);
	return true;
}

CDateTime::CDateTime() : tp(std::chrono::system_clock::now()) 
{
}

CDateTime::CDateTime(const CDateTime &o) : tp(o.tp) 
{
}

CDateTime &CDateTime::operator=(const CDateTime &o)
{
	tp = o.tp;
	return *this;
}

asUINT CDateTime::getYear() const
{
	tm local = time_point_to_tm(tp);
	return local.tm_year + 1900;
}

asUINT CDateTime::getMonth() const
{
	tm local = time_point_to_tm(tp);
	return local.tm_mon + 1;
}

asUINT CDateTime::getDay() const
{
	tm local = time_point_to_tm(tp);
	return local.tm_mday;
}

asUINT CDateTime::getHour() const
{
	tm local = time_point_to_tm(tp);
	return local.tm_hour;
}

asUINT CDateTime::getMinute() const
{
	tm local = time_point_to_tm(tp);
	return local.tm_min;
}

asUINT CDateTime::getSecond() const
{
	tm local = time_point_to_tm(tp);
	return local.tm_sec;
}

bool CDateTime::setDate(asUINT year, asUINT month, asUINT day)
{
	tm local = time_point_to_tm(tp);
	local.tm_year = int(year) - 1900;
	local.tm_mon = month - 1;
	local.tm_mday = day;

	return tm_to_time_point(local, tp);
}

bool CDateTime::setTime(asUINT hour, asUINT minute, asUINT second)
{
	tm local = time_point_to_tm(tp);
	local.tm_hour = hour;
	local.tm_min = minute;
	local.tm_sec = second;

	return tm_to_time_point(local, tp);
}

CDateTime::CDateTime(asUINT year, asUINT month, asUINT day, asUINT hour, asUINT minute, asUINT second)
{
	tm local;
	local.tm_year = int(year) - 1900;
	local.tm_mon = month - 1;
	local.tm_mday = day;
	local.tm_hour = hour;
	local.tm_min = minute;
	local.tm_sec = second;
	if (!tm_to_time_point(local, tp))
	{
		// TODO: How should the user know the input was wrong? Throw exception or have a flag? Set to epoch?
		tp = std::chrono::system_clock::now();
	}
}

asINT64 CDateTime::operator-(const CDateTime &dt) const
{
	return (tp - dt.tp).count() / std::chrono::system_clock::period::den * std::chrono::system_clock::period::num;
}

CDateTime CDateTime::operator+(asINT64 seconds) const
{
	CDateTime dt(*this);
	dt.tp += std::chrono::system_clock::duration(seconds * std::chrono::system_clock::period::den / std::chrono::system_clock::period::num);
	return dt;
}

CDateTime &CDateTime::operator+=(asINT64 seconds)
{
	tp += std::chrono::system_clock::duration(seconds * std::chrono::system_clock::period::den / std::chrono::system_clock::period::num);
	return *this;
}

CDateTime operator+(asINT64 seconds, const CDateTime &other)
{
	return other + seconds;
}

CDateTime CDateTime::operator-(asINT64 seconds) const
{
	return *this + -seconds;
}

CDateTime &CDateTime::operator-=(asINT64 seconds)
{
	return *this += -seconds;
}

CDateTime operator-(asINT64 seconds, const CDateTime &other)
{
	return other + -seconds;
}

bool CDateTime::operator==(const CDateTime &other) const
{
	return tp == other.tp;
}

bool CDateTime::operator<(const CDateTime &other) const
{
	return tp < other.tp;
}

static int opCmp(const CDateTime &a, const CDateTime &b)
{
	if (a < b) return -1;
	if (a == b) return 0;
	return 1;
}

static void Construct(CDateTime *mem)
{
	new(mem) CDateTime();
}

static void ConstructCopy(CDateTime *mem, const CDateTime &o)
{
	new(mem) CDateTime(o);
}

static void ConstructSet(CDateTime *mem, asUINT year, asUINT month, asUINT day, asUINT hour, asUINT minute, asUINT second)
{
	new(mem) CDateTime(year, month, day, hour, minute, second);
}

void RegisterScriptDateTime(asIScriptEngine *engine)
{
	// TODO: Add support for generic calling convention
	assert(strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") == 0);

	int r = engine->RegisterObjectType("datetime", sizeof(CDateTime), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<CDateTime>()); assert(r >= 0);

	r = engine->RegisterObjectBehaviour("datetime", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("datetime", asBEHAVE_CONSTRUCT, "void f(const datetime &in)", asFUNCTION(ConstructCopy), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("datetime", asBEHAVE_CONSTRUCT, "void f(uint, uint, uint, uint = 0, uint = 0, uint = 0)", asFUNCTION(ConstructSet), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime &opAssign(const datetime &in)", asMETHOD(CDateTime, operator=), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "uint get_year() const property", asMETHOD(CDateTime, getYear), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "uint get_month() const property", asMETHOD(CDateTime, getMonth), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "uint get_day() const property", asMETHOD(CDateTime, getDay), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "uint get_hour() const property", asMETHOD(CDateTime, getHour), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "uint get_minute() const property", asMETHOD(CDateTime, getMinute), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "uint get_second() const property", asMETHOD(CDateTime, getSecond), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "bool setDate(uint year, uint month, uint day)", asMETHOD(CDateTime, setDate), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "bool setTime(uint hour, uint minute, uint second)", asMETHOD(CDateTime, setTime), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "int64 opSub(const datetime &in) const", asMETHODPR(CDateTime, operator-, (const CDateTime &other) const, asINT64), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime opAdd(int64 seconds) const", asMETHOD(CDateTime, operator+), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime opAdd_r(int64 seconds) const", asFUNCTIONPR(operator+, (asINT64 seconds, const CDateTime &other), CDateTime), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime &opAddAssign(int64 seconds)", asMETHOD(CDateTime, operator+=), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime opSub(int64 seconds) const", asMETHODPR(CDateTime, operator-, (asINT64) const, CDateTime), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime opSub_r(int64 seconds) const", asFUNCTIONPR(operator-, (asINT64 seconds, const CDateTime &other), CDateTime), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "datetime &opSubAssign(int64 seconds)", asMETHOD(CDateTime, operator-=), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "bool opEquals(const datetime &in) const", asMETHODPR(CDateTime, operator==, (const CDateTime &other) const, bool), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("datetime", "int opCmp(const datetime &in) const", asFUNCTION(opCmp), asCALL_CDECL_OBJFIRST); assert(r >= 0);
}

END_AS_NAMESPACE
