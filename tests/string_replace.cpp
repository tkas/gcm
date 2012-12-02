#include <stdio.h>
#include <stl/String.h>

using namespace GCM;

int main(void) {
	String test("Test\ntest2\ntest3");
	String replaced = test->replace("\n", "<br />\n");
	printf("Input:\n%s\nOutput:\n%s\nEnd.\n", test->c_str(), replaced->c_str());
	
	return EXIT_SUCCESS;
}
