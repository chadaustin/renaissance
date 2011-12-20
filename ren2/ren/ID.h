#pragma once

namespace ren {
    class ID {
    public:
        ID()
            : px(new unsigned(1))
        {}

        ~ID() {
            if (0 == --*px) {
                delete px;
            }
        }

        ID(const ID& id)
            : px(id.px)
        {
            ++*px;
        }
        ID& operator=(const ID& id) {
            if (px != id.px) {
                if (0 == --*px) {
                    delete px;
                }
                px = id.px;
                ++*px;
            }
            return *this;
        }

        bool operator==(ID& rhs) const {
            return px == rhs.px;
        }
        bool operator!=(ID& rhs) const {
            return px != rhs.px;
        }
        bool operator<(ID& rhs) const {
            return px < rhs.px;
        }
        bool operator<=(ID& rhs) const {
            return px <= rhs.px;
        }
        bool operator>(ID& rhs) const {
            return px > rhs.px;
        }
        bool operator>=(ID& rhs) const {
            return px >= rhs.px;
        }

    private:
        unsigned* px;
    };
}
