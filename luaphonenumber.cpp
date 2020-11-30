#include <cassert>
#include <iostream>
#include <string>
#include <cstdio>

#include "phonenumbers/phonenumber.pb.h"
#include "phonenumbers/phonenumberutil.h"
#include "phonenumbers/geocoding/phonenumber_offline_geocoder.h"

extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

lua_State* L;

using i18n::phonenumbers::PhoneNumber;
using i18n::phonenumbers::PhoneNumberUtil;
using i18n::phonenumbers::PhoneNumberOfflineGeocoder;

using std::string;

const PhoneNumberUtil& phone_util = *PhoneNumberUtil::GetInstance();

PhoneNumber _parse(const char* input, const char *default_country) {
  PhoneNumber number;

  phone_util.Parse(input, default_country, &number);

  return number;
}

const char *_get_country(PhoneNumber number) {
  string _country;

  phone_util.GetRegionCodeForNumber(number, &_country);

  return strdup(_country.c_str());
}

const char *_get_location(PhoneNumber number, const char *language, const char *country) {
  string location;

  location = PhoneNumberOfflineGeocoder().GetDescriptionForNumber(number, icu::Locale(language, country));

  if (!location.empty())
  	return strdup(location.c_str());
  else
	  return "";
}

const char *_get_type(PhoneNumber number) {
  PhoneNumberUtil::PhoneNumberType type;

  type = phone_util.GetNumberType(number);
  switch(type) {
    case PhoneNumberUtil::FIXED_LINE:
      return "FIXED_LINE";
    case PhoneNumberUtil::FIXED_LINE_OR_MOBILE:
      return "FIXED_LINE_OR_MOBILE";
    case PhoneNumberUtil::MOBILE:
      return "MOBILE";
    case PhoneNumberUtil::PAGER:
      return "PAGER";
    case PhoneNumberUtil::PERSONAL_NUMBER:
      return "PERSONAL_NUMBER";
    case PhoneNumberUtil::PREMIUM_RATE:
      return "PREMIUM_RATE";
    case PhoneNumberUtil::SHARED_COST:
      return "SHARED_COST";
    case PhoneNumberUtil::TOLL_FREE:
      return "TOLL_FREE";
    case PhoneNumberUtil::UAN:
      return "UAN";
    case PhoneNumberUtil::UNKNOWN:
      return "UNKNOWN";
    case PhoneNumberUtil::VOICEMAIL:
      return "VOICEMAIL";
    case PhoneNumberUtil::VOIP:
      return "VOIP";
    default:
      return "UNKNOWN";
      break;
  }
}

const char* _format(PhoneNumber number, const char *pattern) {
  string formatted_number;

  PhoneNumberUtil::PhoneNumberFormat format;

  if(strcmp(pattern, "E164") == 0) {
    format = PhoneNumberUtil::E164;
  } else if(strcmp(pattern, "INTERNATIONAL") == 0) {
    format = PhoneNumberUtil::INTERNATIONAL;
  } else if(strcmp(pattern, "NATIONAL") == 0) {
    format = PhoneNumberUtil::NATIONAL;
  } else if(strcmp(pattern, "RFC3966") == 0) {
    format = PhoneNumberUtil::RFC3966;
  } else
    return 0;

  phone_util.Format(number, format, &formatted_number);

  return strdup(formatted_number.c_str());
}

extern "C" int get_country(lua_State* L) {
  PhoneNumber number;

  const char *input   = luaL_checkstring(L, 1);
  const char *country = luaL_checkstring(L, 2);

  number = _parse(input, country);

  const char * s = _get_country(number);
  lua_pushstring(L, s);
  free((void*)s);

  return 1;
}

extern "C" int get_location(lua_State* L) {
  PhoneNumber number;

  const char *input    = luaL_checkstring(L, 1);
  const char *country  = luaL_checkstring(L, 2);
  const char *language = luaL_checkstring(L, 3);
  const char *_country = luaL_checkstring(L, 4);

  number = _parse(input, country);

  const char * s = _get_location(number, language, _country);
  lua_pushstring(L, s);
  free((void*)s);
  
  return 1;
}

extern "C" int get_type(lua_State* L) {
  PhoneNumber number;

  const char *input   = luaL_checkstring(L, 1);
  const char *country = luaL_checkstring(L, 2);

  number = _parse(input, country);

  lua_pushstring(L, _get_type(number));

  return 1;
}

extern "C" int format(lua_State* L) {
  PhoneNumber number;

  const char *input    = luaL_checkstring(L, 1);
  const char *country  = luaL_checkstring(L, 2);
  const char *__format = luaL_checkstring(L, 3);

  number = _parse(input, country);

  const char * s = _format(number, __format);
  lua_pushstring(L, s);
  free((void*)s);

  return 1;
}

extern "C" int parse(lua_State* L) {
  PhoneNumber number;
  string      formatted_number;

  const char *input    = luaL_checkstring(L, 1);
  const char *country  = luaL_checkstring(L, 2);
  const char *language = luaL_checkstring(L, 3);
  const char *_country = luaL_checkstring(L, 4);

  //FILE* fp = std::fopen("/tmp/test.txt", "w+");
  //std::fputs("Start", fp);

  number = _parse(input, country);

  lua_createtable(L, 0, 8);

  lua_pushstring(L, _get_country(number));
  lua_setfield (L, -2, "country");
  //std::fputs(_get_country(number),fp);

  lua_pushinteger(L, number.country_code());
  lua_setfield (L, -2, "country_prefix");

  lua_pushstring(L, _get_location(number, language, _country));
  lua_setfield (L, -2, "location");

  lua_pushstring(L, _get_type(number));
  lua_setfield (L, -2, "type");

  lua_pushstring(L, _format(number, "E164"));
  lua_setfield (L, -2, "E164");

  lua_pushstring(L, _format(number, "INTERNATIONAL"));
  lua_setfield (L, -2, "INTERNATIONAL");

  lua_pushstring(L, _format(number, "NATIONAL"));
  lua_setfield (L, -2, "NATIONAL");

  lua_pushstring(L, _format(number, "RFC3966"));
  lua_setfield (L, -2, "RFC3966");

  //std::fclose(fp);
  return 1;
}

extern "C" int luaopen_luaphonenumber(lua_State* L) {
  static const struct luaL_Reg phonenumber[] = {
    {"get_country",     get_country},
    {"get_location",    get_location},
    {"get_type",        get_type},
    {"format",          format},
    {"parse",           parse},
    {NULL,              NULL}
  };

  luaL_register(L, "phonenumber", phonenumber);

  return 1;
}