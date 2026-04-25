// Демонстрация classname(), isA() и dynamic_cast
#include <iostream>
#include <string>

using namespace std;

class Base {
protected:
    string color;
    int id;
    
public:
    static int totalObjects;
    
    Base() : color("blue") {
        id = ++totalObjects;
        cout << "Base(): создан объект id=" << id << endl;
    }
    
    Base(string color_input) : color(color_input) {
        id = ++totalObjects;
        cout << "Base(string): создан объект id=" << id << endl;
    }
    
    Base(const Base& other) : color(other.color) {
        id = ++totalObjects;
        cout << "Base(Base&): создан объект id=" << id 
             << " (копия из id=" << other.id << ")" << endl;
    }
    
    Base(Base* other) : color(other->color) {
        id = ++totalObjects;
        cout << "Base(Base*): создан объект id=" << id 
             << " из указателя на id=" << other->id << endl;
    }
    
    virtual ~Base() {
        totalObjects--;
        cout << "~Base(): удалён объект id=" << id << endl;
    }
    
    virtual string classname() const = 0;
    
    virtual bool isA(const string& name) const {
        return name == "Base";
    }
    
    int getId() const { return id; }
};

int Base::totalObjects = 0;

class Desc : public Base {
private:
    double value;
    int descID;
    
public:
    static int totalDesc;
    
    Desc() : Base(), value(10.0) {
        descID = ++totalDesc;
        cout << "Desc(): создан потомок id=" << descID << endl;
    }
    
    Desc(double val, string color) : Base(color), value(val) {
        descID = ++totalDesc;
        cout << "Desc(double,string): создан потомок id=" << descID << endl;
    }
    
    Desc(const Desc& other) : Base(other), value(other.value) {
        descID = ++totalDesc;
        cout << "Desc(Desc&): создан потомок id=" << descID 
             << " (копия из id=" << other.descID << ")" << endl;
    }
    
    Desc(Desc* other) : Base(other), value(other->value) {
        descID = ++totalDesc;
        cout << "Desc(Desc*): создан потомок id=" << descID 
             << " из указателя на потомка id=" << other->descID << endl;
    }
    
    ~Desc() override {
        totalDesc--;
        cout << "~Desc(): удалён потомок id=" << descID << endl;
    }
    
    string classname() const override {
        return "Desc";
    }
    
    bool isA(const string& name) const override {
        if (name == "Desc") return true;
        return Base::isA(name);
    }
    
    double getValue() const { return value; }
    void setValue(double v) { value = v; }
};

int Desc::totalDesc = 0;

void demonstrateClassnameAndIsA() {
    cout << "\n1. classname() и isA()" << endl;
    
    Base* basePtr = new Desc(5.0, "wood");
    
    cout << "\nclassname(): " << basePtr->classname() << endl;
    cout << "isA(\"Base\"): " << basePtr->isA("Base") << endl;
    cout << "isA(\"Desc\"): " << basePtr->isA("Desc") << endl;
    
    delete basePtr;
}

void demonstrateDangerousCast() {
    cout << "\n2. Опасное приведение C-style" << endl;
    
    Base* basePtr = new Desc(10.0, "plastic");
    cout << "\nСоздан Desc, указатель на Base" << endl;
    
    Desc* badCast = (Desc*)basePtr;
    cout << "C-style cast к Desc*: ";
    badCast->setValue(25.0);
    cout << "Новое значение: " << badCast->getValue() << endl;
    
    delete basePtr;
}

void demonstrateSafeCastWithIsA() {
    cout << "\n3. Безопасное приведение через isA()" << endl;
    
    Base* basePtr = new Desc(7.0, "stone");
    
    cout << "Объект classname() = " << basePtr->classname() << endl;
    
    if (basePtr->isA("Desc")) {
        Desc* d = (Desc*)basePtr;
        cout << "isA() вернул true -> безопасно приводим к Desc*" << endl;
        cout << "Значение = " << d->getValue() << endl;
        d->setValue(12.5);
        cout << "Новое значение = " << d->getValue() << endl;
    }
    
    delete basePtr;
}

void demonstrateDynamicCast() {
    cout << "\n4. dynamic_cast" << endl;
    
    Base* basePtr = new Desc(15.0, "platinum");
    cout << "\nСоздан Desc, указатель на Base" << endl;
    
    Desc* descPtr = dynamic_cast<Desc*>(basePtr);
    if (descPtr) {
        cout << "dynamic_cast<Desc*> успешен!" << endl;
        cout << "Значение = " << descPtr->getValue() << endl;
    } else {
        cout << "dynamic_cast<Desc*> вернул nullptr" << endl;
    }
    
    delete basePtr;
    
    cout << "\ndynamic_cast для ссылок" << endl;
    
    Desc desc(20.0, "gold");
    Base& baseRef = desc;
    
    try {
        Desc& descRef = dynamic_cast<Desc&>(baseRef);
        cout << "dynamic_cast<Desc&> успешен!" << endl;
        cout << "Значение = " << descRef.getValue() << endl;
    }
    catch (const bad_cast& e) {
        cout << "dynamic_cast<Desc&> выбросил исключение bad_cast" << endl;
    }
}

int main() {
    demonstrateClassnameAndIsA();
    demonstrateDangerousCast();
    demonstrateSafeCastWithIsA();
    demonstrateDynamicCast();
    
    return 0;
}