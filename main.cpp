#include <iostream>
#include <memory>
#include <string>

using std::cout;
using std::endl;
using std::string;

class Base {
public:
    Base() : id_(++next_id_) {
        cout << "[CTOR] Base() id=" << id_ << endl;
    }

    Base(Base* obj) : id_(++next_id_) {
        cout << "[CTOR] Base(Base*) id=" << id_ << " from=" << (obj ? obj->id_ : -1) << endl;
    }

    Base(Base& obj) : id_(++next_id_) {
        cout << "[CTOR] Base(Base&) id=" << id_ << " from=" << obj.id_ << endl;
    }

    Base(const Base& obj) : id_(++next_id_) {
        cout << "[CTOR] Base(const Base&) id=" << id_ << " from=" << obj.id_ << endl;
    }

    virtual ~Base() {
        cout << "[DTOR] ~Base() id=" << id_ << endl;
    }

    int id() const {
        return id_;
    }

    // Невиртуальный метод: выбор реализации зависит от типа указателя/ссылки.
    void nonVirtualMethod() const {
        cout << "[CALL] Base::nonVirtualMethod id=" << id_ << endl;
    }

    // Виртуальный метод: выбор реализации зависит от реального типа объекта.
    virtual void virtualMethod() const {
        cout << "[CALL] Base::virtualMethod id=" << id_ << endl;
    }

    // Метод 1 вызывает метод 2 (невиртуальный): в потомке полиморфизма не будет.
    void method1CallsMethod2NonVirtual() const {
        cout << "[CALL] Base::method1CallsMethod2NonVirtual -> ";
        method2NonVirtual();
    }

    // Метод 1 вызывает метод 2 (виртуальный): в потомке будет полиморфизм.
    void method1CallsMethod2Virtual() const {
        cout << "[CALL] Base::method1CallsMethod2Virtual -> ";
        method2Virtual();
    }

    void method2NonVirtual() const {
        cout << "Base::method2NonVirtual id=" << id_ << endl;
    }

    virtual void method2Virtual() const {
        cout << "Base::method2Virtual id=" << id_ << endl;
    }

    virtual string classname() const {
        return "Base";
    }

    virtual bool isA(const string& class_name) const {
        return class_name == "Base";
    }

protected:
    static int next_id_;
    int id_;
};

int Base::next_id_ = 0;

class Desc : public Base {
public:
    Desc() : Base() {
        cout << "[CTOR] Desc() id=" << id_ << endl;
    }

    Desc(Desc* obj) : Base(obj) {
        cout << "[CTOR] Desc(Desc*) id=" << id_ << " from=" << (obj ? obj->id_ : -1) << endl;
    }

    Desc(Desc& obj) : Base(obj) {
        cout << "[CTOR] Desc(Desc&) id=" << id_ << " from=" << obj.id_ << endl;
    }

    Desc(const Desc& obj) : Base(obj) {
        cout << "[CTOR] Desc(const Desc&) id=" << id_ << " from=" << obj.id_ << endl;
    }

    ~Desc() override {
        cout << "[DTOR] ~Desc() id=" << id_ << endl;
    }

    void nonVirtualMethod() const {
        cout << "[CALL] Desc::nonVirtualMethod id=" << id_ << endl;
    }

    void virtualMethod() const override {
        cout << "[CALL] Desc::virtualMethod id=" << id_ << endl;
    }

    void method2NonVirtual() const {
        cout << "Desc::method2NonVirtual id=" << id_ << endl;
    }

    void method2Virtual() const override {
        cout << "Desc::method2Virtual id=" << id_ << endl;
    }

    void descOnly() const {
        cout << "[CALL] Desc::descOnly id=" << id_ << endl;
    }

    string classname() const override {
        return "Desc";
    }

    bool isA(const string& class_name) const override {
        return class_name == "Desc" || Base::isA(class_name);
    }
};

void printHeader(const string& title) {
    cout << "\n========== " << title << " ==========" << endl;
}

namespace ParamDemo {

void func1(Base obj) {
    cout << "[func1] parameter Base obj (copy), id=" << obj.id() << endl;
    obj.virtualMethod();
    Desc* casted = dynamic_cast<Desc*>(&obj);
    cout << "[func1] dynamic_cast<Desc*>(&obj) -> " << (casted ? "not null" : "null") << endl;
}

void func2(Base* obj) {
    cout << "[func2] parameter Base* obj, id=" << (obj ? obj->id() : -1) << endl;
    if (!obj) {
        return;
    }

    obj->virtualMethod();

    if (obj->isA("Desc")) {
        cout << "[func2] isA says Desc -> manual cast allowed" << endl;
        static_cast<Desc*>(obj)->descOnly();
    } else {
        cout << "[func2] isA says not Desc -> manual cast forbidden" << endl;
    }

    Desc* safe = dynamic_cast<Desc*>(obj);
    cout << "[func2] dynamic_cast<Desc*>(obj) -> " << (safe ? "not null" : "null") << endl;
}

void func3(Base& obj) {
    cout << "[func3] parameter Base& obj, id=" << obj.id() << endl;
    obj.virtualMethod();

    Desc* safe = dynamic_cast<Desc*>(&obj);
    cout << "[func3] dynamic_cast<Desc*>(&obj) -> " << (safe ? "not null" : "null") << endl;
    if (safe) {
        safe->descOnly();
    }
}

}  // namespace ParamDemo

namespace ReturnDemo {

Base func1() {
    static Base local_static;
    cout << "[ret func1] return Base by value from static local id=" << local_static.id() << endl;
    return local_static;
}

Base* func2() {
    static Base local_static;
    cout << "[ret func2] return Base* to static local id=" << local_static.id() << endl;
    return &local_static;
}

Base& func3() {
    static Base local_static;
    cout << "[ret func3] return Base& to static local id=" << local_static.id() << endl;
    return local_static;
}

Base func4() {
    Base* dyn = new Base();
    cout << "[ret func4] return Base by value from dynamic id=" << dyn->id() << endl;
    Base result = *dyn;
    delete dyn;
    return result;
}

Base* func5() {
    Base* dyn = new Base();
    cout << "[ret func5] return Base* (dynamic) id=" << dyn->id() << endl;
    return dyn;
}

Base& func6() {
    Base* dyn = new Base();
    cout << "[ret func6] return Base& to dynamic id=" << dyn->id() << endl;
    return *dyn;
}

}  // namespace ReturnDemo

void takeUnique(std::unique_ptr<Base> ptr) {
    cout << "[takeUnique] got owner of id=" << (ptr ? ptr->id() : -1) << endl;
}

void takeShared(const std::shared_ptr<Base>& ptr) {
    cout << "[takeShared] use_count=" << ptr.use_count() << " id=" << ptr->id() << endl;
}

std::unique_ptr<Base> makeUniqueBase() {
    auto ptr = std::make_unique<Base>();
    cout << "[makeUniqueBase] created id=" << ptr->id() << endl;
    return ptr;
}

std::shared_ptr<Base> makeSharedBase() {
    auto ptr = std::make_shared<Base>();
    cout << "[makeSharedBase] created id=" << ptr->id() << ", use_count=" << ptr.use_count() << endl;
    return ptr;
}

void demoMethodHidingAndVirtual() {
    printHeader("Перекрытие и виртуальность");

    Base b;
    Desc d;

    Base* pb_base = &b;
    Base* pb_desc = &d;
    Desc* pd_desc = &d;

    cout << "\n[1] Невиртуальный метод:" << endl;
    pb_base->nonVirtualMethod();
    pb_desc->nonVirtualMethod();
    pd_desc->nonVirtualMethod();

    cout << "\n[2] Виртуальный метод:" << endl;
    pb_base->virtualMethod();
    pb_desc->virtualMethod();
    pd_desc->virtualMethod();

    cout << "\n[3] method1 -> method2 (невиртуальный):" << endl;
    d.method1CallsMethod2NonVirtual();

    cout << "\n[4] method1 -> method2 (виртуальный):" << endl;
    d.method1CallsMethod2Virtual();
}

void demoRequiredConstructorsAndVirtualDtor() {
    printHeader("Требуемые конструкторы и виртуальный деструктор");

    Base b1;
    Base b2(&b1);
    Base b3(b1);

    Desc d1;
    Desc d2(&d1);
    Desc d3(d1);

    cout << "\n[virtual dtor] delete Desc via Base*" << endl;
    Base* poly = new Desc();
    delete poly;
}

void demoClassnameAndIsAAndCasts() {
    printHeader("classname, isA, dynamic_cast и ручная проверка");

    Base b;
    Desc d;

    Base* p1 = &b;
    Base* p2 = &d;

    cout << "p1->classname()=" << p1->classname() << ", isA(Base)=" << p1->isA("Base")
         << ", isA(Desc)=" << p1->isA("Desc") << endl;
    cout << "p2->classname()=" << p2->classname() << ", isA(Base)=" << p2->isA("Base")
         << ", isA(Desc)=" << p2->isA("Desc") << endl;

    cout << "\n[Проблема classname] Desc является Base, но classname()==\"Base\" для Desc не выполнится." << endl;

    cout << "\n[Опасное приведение] static_cast<Desc*>(p1) без проверки запрещено (UB при использовании)." << endl;
    if (p1->isA("Desc")) {
        static_cast<Desc*>(p1)->descOnly();
    } else {
        cout << "Пропускаем manual cast, потому что p1 не Desc." << endl;
    }

    Desc* safe1 = dynamic_cast<Desc*>(p1);
    Desc* safe2 = dynamic_cast<Desc*>(p2);
    cout << "dynamic_cast for p1 -> " << (safe1 ? "not null" : "null") << endl;
    cout << "dynamic_cast for p2 -> " << (safe2 ? "not null" : "null") << endl;
    if (safe2) {
        safe2->descOnly();
    }
}

void demoParamsLifecycle() {
    printHeader("Передача параметров в функции");

    Base b;
    Desc d;

    cout << "\n--- Передаем Base ---" << endl;
    ParamDemo::func1(b);
    ParamDemo::func2(&b);
    ParamDemo::func3(b);

    cout << "\n--- Передаем Desc ---" << endl;
    ParamDemo::func1(d);
    ParamDemo::func2(&d);
    ParamDemo::func3(d);
}

void demoReturnLifecycle() {
    printHeader("Возврат объектов из функций");

    cout << "\n[static local]" << endl;
    Base a = ReturnDemo::func1();
    Base* b = ReturnDemo::func2();
    Base& c = ReturnDemo::func3();
    cout << "received a.id=" << a.id() << ", b.id=" << b->id() << ", c.id=" << c.id() << endl;

    cout << "\n[dynamic new]" << endl;
    Base d = ReturnDemo::func4();
    Base* e = ReturnDemo::func5();
    Base& f = ReturnDemo::func6();
    cout << "received d.id=" << d.id() << ", e.id=" << e->id() << ", f.id=" << f.id() << endl;

    cout << "\n[cleanup dynamic results]" << endl;
    delete e;
    delete &f;
}

void demoSmartPointers() {
    printHeader("unique_ptr и shared_ptr");

    {
        cout << "\n[unique_ptr локально]" << endl;
        auto u = std::make_unique<Desc>();
        cout << "owner id=" << u->id() << endl;
        takeUnique(std::move(u));
        cout << "after move u==nullptr -> " << (u == nullptr) << endl;
    }

    {
        cout << "\n[unique_ptr из функции]" << endl;
        auto u2 = makeUniqueBase();
        cout << "returned unique_ptr id=" << u2->id() << endl;
    }

    {
        cout << "\n[shared_ptr]" << endl;
        auto s1 = makeSharedBase();
        cout << "after return use_count=" << s1.use_count() << endl;

        auto s2 = s1;
        cout << "after copy use_count=" << s1.use_count() << endl;

        takeShared(s1);
        cout << "after function use_count=" << s1.use_count() << endl;
    }
}

int main() {
    printHeader("ЛР5. Жизненный цикл объектов C++ и виртуальность");

    demoMethodHidingAndVirtual();
    demoRequiredConstructorsAndVirtualDtor();
    demoClassnameAndIsAAndCasts();
    demoParamsLifecycle();
    demoReturnLifecycle();
    demoSmartPointers();

    printHeader("Завершение программы");
    return 0;
}
