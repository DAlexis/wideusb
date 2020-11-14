#include "modules/gps/point.hpp"
#include "utilities.hpp"

Point::Point()
{
    zerify(time);
}

tm Point::get_tm() const
{
    tm result = *localtime(&time.tv_sec);
    return result;
}
