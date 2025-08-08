#pragma once

class InputInterceptor {
public:
    static InputInterceptor& Instance();

    void Init();
    void Shutdown();
    void SetBlockSprint(bool block);
    bool IsSprintBlocked() const;


private:
    InputInterceptor();
    bool blockSprint = false;
};
