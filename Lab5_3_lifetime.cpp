// Демонстрация передачи/возврата объектов и умных указателей
#include <iostream>
#include <string>
#include <memory>

using namespace std;

class Base {
protected:
    string material;
    int id;
    
public:
    static int totalObjects;
    
    Base() : material("steel") {
        id = ++totalObjects;
        cout << "Base(): создан объект id=" << id << endl;
    }
    
    Base(string material_input) : material(material_input) {
        id = ++totalObjects;
        cout << "Base(string): создан объект id=" << id << endl;
    }
    
    Base(const Base& other) : material(other.material) {
        id = ++totalObjects;
        cout << "Base(Base&): создан объект id=" << id 
             << " (копия из id=" << other.id << ")" << endl;
    }
    
    Base(Base* other) : material(other->material) {
        id = ++totalObjects;
        cout << "Base(Base*): создан объект id=" << id 
             << " из указателя на id=" << other->id << endl;
    }
    
    virtual ~Base() {
        totalObjects--;
        cout << "~Base(): удалён объект id=" << id << endl;
    }
    
    virtual void draw() const {
        cout << "Base::draw()" << endl;
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
        cout << "Desc(): создан потомок id=" << descID << ", значение=" << value << endl;
    }
    
    Desc(double val, string material) : Base(material), value(val) {
        descID = ++totalDesc;
        cout << "Desc(double,string): создан потомок id=" << descID 
             << ", значение=" << value << endl;
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
        cout << "~Desc(): удалён потомок id=" << descID << ", значение=" << value << endl;
    }
    
    void draw() const override {
        cout << "Desc::draw(): значение=" << value << endl;
    }
    
    double getValue() const { return value; }
    void setValue(double v) { value = v; }
};

int Desc::totalDesc = 0;

void func1(Base obj) {
    cout << "  func1: получила объект по значению, id=" << obj.getId() << endl;
    obj.draw();
}

void func2(Base* obj) {
    cout << "  func2: получила указатель, id=" << obj->getId() << endl;
    obj->draw();
}

void func3(Base& obj) {
    cout << "  func3: получила ссылку, id=" << obj.getId() << endl;
    obj.draw();
}

Base func1_returnLocal() {
    Desc local(5.0, "wood");
    cout << "  func1_returnLocal: возвращаем локальный объект по значению" << endl;
    return local;  
}

Base* func2_returnLocal() {
    Desc local(7.0, "plastic");
    cout << "  func2_returnLocal: возвращаем УКАЗАТЕЛЬ на локальный объект (ОПАСНО!)" << endl;
    return &local; 
}

Base& func3_returnLocal() {
    Desc local(10.0, "steel");
    cout << "  func3_returnLocal: возвращаем ССЫЛКУ на локальный объект (ОПАСНО!)" << endl;
    return local;  
}

Base func4_returnDynamic() {
    Desc* ptr = new Desc(15.0, "gold");
    cout << "  func4_returnDynamic: возвращаем динамический объект по значению" << endl;
    return *ptr;  
}

Base* func5_returnDynamic() {
    Desc* ptr = new Desc(20.0, "platinum");
    cout << "  func5_returnDynamic: возвращаем УКАЗАТЕЛЬ на динамический объект" << endl;
    return ptr;   
}

Base& func6_returnDynamic() {
    Desc* ptr = new Desc(25.0, "iron");
    cout << "  func6_returnDynamic: возвращаем ССЫЛКУ на динамический объект" << endl;
    return *ptr;  
}

void demonstrateUniquePtr() {
    cout << "\nunique_ptr" << endl;
    
    unique_ptr<Desc> ptr1 = make_unique<Desc>(30.0, "paper");
    ptr1->draw();
    
    cout << "unique_ptr нельзя скопировать, только переместить" << endl;
    unique_ptr<Desc> ptr2 = move(ptr1);
    
    if (!ptr1) {
        cout << "ptr1 теперь пуст" << endl;
    }
    
    cout << "ptr2-> ";
    ptr2->draw();
    
    cout << "unique_ptr автоматически удалит объект при выходе из функции" << endl;
}

void demonstrateSharedPtr() {
    cout << "\nshared_ptr " << endl;
    
    shared_ptr<Desc> ptr1 = make_shared<Desc>(40.0, "wood");
    cout << "Счётчик ссылок: " << ptr1.use_count() << endl;
    
    {
        shared_ptr<Desc> ptr2 = ptr1;
        cout << "Счётчик ссылок (внутри блока): " << ptr1.use_count() << endl;
    }
    
    cout << "Счётчик ссылок (после выхода из блока): " << ptr1.use_count() << endl;
    cout << "Объект удалится когда счётчик станет 0" << endl;
}

void demonstratePassing() {
    cout << "\n1. ПЕРЕДАЧА ОБЪЕКТОВ В ФУНКЦИИ" << endl;
    
    Desc desc(25.0, "nitrogen");
    cout << "\nСоздан объект Desc id=" << desc.getId() << endl;
    
    cout << "\n Передача по значению (func1)" << endl;
    cout << "  (вызывается конструктор копирования)" << endl;
    func1(desc);
    
    cout << "\nПередача по указателю (func2)" << endl;
    cout << "  (копия не создаётся)" << endl;
    func2(&desc);
    
    cout << "\nПередача по ссылке (func3)" << endl;
    cout << "  (копия не создаётся)" << endl;
    func3(desc);
}

void demonstrateReturn() {
    cout << "\n2. ВОЗВРАТ ОБЪЕКТОВ ИЗ ФУНКЦИЙ" << endl;
    
    cout << "\nФункция 1: возврат локального объекта по значению" << endl;
    Base s1 = func1_returnLocal();
    cout << "  Объект получен, локальный объект уже уничтожен" << endl;
    
    cout << "\nФункция 2: возврат указателя на локальный объект" << endl;
    Base* badPtr = func2_returnLocal();
    cout << "  Указатель получен, но объект уже уничтожен" << endl;
    cout << "  (привело бы к неопределённому поведению)" << endl;
    
    cout << "\nФункция 3: возврат ссылки на локальный объект" << endl;
    Base& badRef = func3_returnLocal();
    cout << "  Ссылка получена, но объект уже уничтожен" << endl;
    cout << "  (привело бы к неопределённому поведению)" << endl;
    
    cout << "\nФункция 4: возврат динамического объекта по значени" << endl;
    Base s2 = func4_returnDynamic();
    cout << "  Объект скопирован, оригинал не удалён -> утечка памяти" << endl;
    
    cout << "\nФункция 5: возврат указателя на динамический объект" << endl;
    Base* goodPtr = func5_returnDynamic();
    cout << "  Указатель получен, объект существует" << endl;
    goodPtr->draw();
    delete goodPtr;
    cout << "  Объект удалён вручную" << endl;
    
    cout << "\nФункция 6: возврат ссылки на динамический объект" << endl;
    Base& goodRef = func6_returnDynamic();
    cout << "  Ссылка получена, объект существует" << endl;
    goodRef.draw();
    delete &goodRef;
    cout << "  Объект удалён вручную" << endl;
}

void demonstrateSmartPointers() {
    cout << "\n3. Умные указатели" << endl;
    
    demonstrateUniquePtr();
    demonstrateSharedPtr();
}

int main() {
    demonstratePassing();
    demonstrateReturn();
    demonstrateSmartPointers();

    return 0;
}
