#pragma once

class Square;

class Move
{
public:
    Move(Square* from, Square* to);

    Square* getFrom() const;
    Square* getTo() const;

private:
    Square* m_from;
    Square* m_to;
};