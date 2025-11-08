#include <stdio.h>

void func() {
    const char *msg1 = "HelloWorld\n";
	write(1, msg1,11);
}

int main() {
    func();
    return 0;
}
