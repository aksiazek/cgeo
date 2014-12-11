#ifndef COMMON_HPP
#define COMMON_HPP

#include <cmath>
#include <string>

namespace gogui {

    class GeoObject {
    public:
        std::string getColor() const {
            return color;
        }

        void setColor(std::string color) {
            GeoObject::color = color;
        }

        enum class Status {
            Normal, Active, Processed
        };

        Status getStatus() const {
            return status;
        }

        void setStatus(Status s) {
            status = s;
        }

        bool compareDouble(const double a, const double b) const{
            return fabs(a - b) < 1e-10;
        }

    private:
        Status status = Status::Normal;
        std::string color = "";
    };


}

#endif // COMMON_HPP
