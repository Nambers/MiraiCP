# 开发者代码规范

## C++部分

* 花括号开头在行尾，结束在新行。可以用简短的单行写完的例外。

* 禁用`dynamic_cast`及任何RTTI相关的函数。

* 构造函数数据要被使用时，优先使用值传递并使用`std::move`，而非const引用。只读时用const引用。

* 尽可能使用枚举而不是int作为类型判别；尽可能使用新类型定义而不是使用默认类型，如`QQID`替换`unsigned long long`。

* 有虚函数，可以被继承或继承并加入了新的成员变量的类或结构体，必须有虚的析构函数；否则，应标记为`final`，在其他类型中使用组合模式；继承没有虚析构函数的标准库类或结构体时，禁用多态。

* 类或结构体的内部按照一定顺序排列。按照如下规则

  * 推荐的顺序：类型定义->静态成员->成员->构造、析构函数->运算符重载->方法，采用其他顺序如方法在成员前也可以
  * 对于不同类型的内容，前面需要加入public或private限定符，就算前一个类型的内容限定符和当前的相同。开头可以不写
  * 非静态成员的声明按照如下方法排列：size>=8的按任意顺序排在最前，其他的在后面尽可能按size从大到小排列

  一个合适的类型定义如下：

  ```C++
  class SomeType : public SomeSuperType {
      // typedefs
      typedef SomeSuperType Super;
      using string = std::string;
      
  public:
      struct NestedSomeType{
          // some types ...
      };
      
      // static members
  public:
      static int x;
  private:
      static float y;
      
      // members
  private:
      QQID id; // size >= 8 (actual size: 8)
      double z; // size >= 8 (actual size: 8)
      void* ptr; // size >= 8 (actual size: 8, since MiraiCP requires 64 bit)
      string s; // size >= 8 (actual size: depends)
      float w; // size 4
      unsigned char t; // size 1
      
      // constructors
  protected:
      SomeType() = default;
      
  public:
      SomeType(unsigned char in_val);
      
      //destructor
  public:
      ~SomeType() override = default;
      
      // static methods
  public:
      static auto gety(){return y};
      static void sety(float _y){y = _y};
      
      // overloading operators
  public:
      SomeType& operator=(const SomeType& other);
      SomeType& operator=(SomeType&& other);
      bool operator==(const SomeType& other);
      
      // virtual methods
  public:
      void some_virtualfunc() const override;
  
      // other methods
  public:
      void some_nonvirtualfunc() const;
  };
  ```

* lambda表达式每个值按引用捕获时务必单独捕获，尽可能不要使用`[&]`以带来不必要的debug困难。

* 禁用`std::pair`，除非，不会使用其`first`，`second`且：

  * 在一个局部作用域内定义
  * 或仅用于结构化绑定

  请优先考虑一个新的结构体定义。

* 除非是每个语句都能用简短的单行写完，否则`switch`语句的每个case都必须用花括号包围。可以不用花括号的例子：

  ```c++
  switch(cond){
      case MIRAI_FRIEND:
          do_sth();
          break;
      case MIRAI_MEMBER:
          do_sth();
          break;
      case MIRAI_GROUP:
          do_sth();
          break;
      default:
          break;
  }
  ```

* 当从单一来源获取实参用于初始化对象时，应当将该数据传入构造函数在构造函数内解析，而非先解析该数据再传入构造函数。好的例子：

  ```c++
  Group g(std::move(in_json));
  ```

  坏的例子：

  ```c++
  Group g(in_json["groupid"], in_json["botid"]);
  ```

* 优先使用`std::make_shared<T>(Args&&...)`以及`std::make_unique(Args&&...)`，尽可能不使用`std::shared_ptr<T>::reset(T*)`，`std::shared_ptr<T>::operator=(T*)`，以及`std::shared_ptr<T>::shared_ptr(T*)`；但当使用`std::shared_ptr<T[]>`类型时，反之。

* 内部函数命名带上internal前缀或后缀，避免`send0`，`send1`这类命名。
* 对于平凡可移动、平凡可复制、简单聚合等类型，如果不是被隐式删除的情况下，不要加入默认复制构造、默认移动构造、默认`==`重载等方法。