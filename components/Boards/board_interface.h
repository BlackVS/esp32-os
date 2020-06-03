#pragma once

class Board 
{
    protected:
        Board() = default;

    public:
        virtual ~Board() = default;

    public:
        virtual void init() {};
        virtual void run() {};
};