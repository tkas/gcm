#ifndef _GCM_MODIFIABLE_H
# define _GCM_MODIFIABLE_H

namespace GCM {

template <typename T>
class Modifiable {

    public:
        const T &get() const { return value; }
        T &get() { return value; }

        bool isModified() const { return modified; }
        void clearModified() { modified = false; }

        Modifiable(): value(), modified(false) {}
        Modifiable(T value): value(value), modified(false) {}
        Modifiable(T &value): value(value), modified(false) {}
        Modifiable(const Modifiable<T> &value): value(value.value), modified(value.modified) {}
        Modifiable(T value, bool modified): value(value), modified(modified) {}

        /** Assignment */
        Modifiable<T> &operator=(T &value) {
            this->value = value;
            this->modified = true;

            return *this;
        }

        Modifiable<T> &operator=(Modifiable<T> &value) {
            if (&value == this) return *this;

            this->value = value.value;
            this->modified = true;

            return *this;
        }

        /** Compare */
        bool operator==(const Modifiable<T> &other) const {
            return this->value == other.value;
        }

        bool operator!=(const Modifiable<T> &other) const {
            return this->value != other.value;
        }

        bool operator==(const T &other) const {
            return this->value == other;
        }

        bool operator!=(const T &other) const {
            return this->value != other;
        }

        bool operator<(const Modifiable<T> &other) const {
            return this->value < other.value;
        }

        bool operator>(const Modifiable<T> &other) const {
            return this->value > other.value;
        }

        bool operator<(const T &other) const {
            return this->value < other;
        }

        bool operator>(const T &other) const {
            return this->value > other;
        }

        T operator->() const { return this->value; }

        operator T() const { return this->value; }

        template<typename U>
        Modifiable<U> get() const { return Modifiable<U>(value, modified); }

    protected:
        T value;
        bool modified;
};

template<typename T>
class ModifiableEnum: public Modifiable<T> {
    public:
        ModifiableEnum(const Modifiable<T> &value): Modifiable<T>(value) {}

        operator int() const { return (int)this->value; }

        ModifiableEnum<T> &operator=(T &value) {
            this->value = value;
            this->modified = true;

            return *this;
        }

        ModifiableEnum<T> &operator=(Modifiable<T> &value) {
            if (&value == this) return *this;

            this->value = value.value;
            this->modified = true;

            return *this;
        }

        operator Modifiable<int>() const {
            return Modifiable<int>(this->value, this->modified);
        }
};

}

#endif
