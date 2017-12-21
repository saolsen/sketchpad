#define FOO(name) int name(void)
typedef FOO(Foo);
extern Foo foo;