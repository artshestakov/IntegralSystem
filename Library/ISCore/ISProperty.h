#pragma once
#ifndef _ISPROPERTY_H_INCLUDED
#define _ISPROPERTY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISProperty
{
public:
    static ISProperty& Instance();

    const uint64_t& GetUptime() const;
    void SetUptime(uint64_t uptime);

    const unsigned int& GetServerVersion() const;
    void SetServerVersion(unsigned int server_version);

    bool GetChangeUser() const;
    void SetChangeUser(bool change_version);

    const std::string& GetPathLastDirectory() const;
    void SetPathLastDirectory(const std::string &path_last_directory);

private:
    ISProperty();
    ~ISProperty();
    ISProperty(const ISProperty&) = delete;
    ISProperty(ISProperty&&) = delete;
    ISProperty& operator=(const ISProperty&) = delete;
    ISProperty& operator=(ISProperty&&) = delete;

private:
    uint64_t Uptime;
    unsigned int ServerVersion;
    bool ChangeUser;
    std::string PathLastDirectory;
};
//-----------------------------------------------------------------------------
#endif
