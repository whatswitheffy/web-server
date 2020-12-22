#include <stdio.h> 
#include <string.h> 

typedef struct Student {
	char name[80];
	int math_mark;
	int phys_mark;
} Student;

int main(int argc, char *argv[]) {
	Student StudentList[9] = {
		{"Eva", 5, 5},
		{"Alex", 3, 2},
		{"Alen", 10, 10},
		{"Dauren", 5, 5},
		{"Tair", 6, 4},
		{"Lera", 8, 9},
		{"Tamerlan", 5, 9},
		{"Aigerim", 4, 8}, 
		{"Anna", 9, 10}
	};
	char *name, *subject;
	int mark = -1;

	printf("<html>\n");
	if (argc == 1) {
		printf("<h1>I can find student's mark!</h1>\n");
		printf("</html>\n");
		return -1;
	}
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "name") == 0) {
			name = argv[i + 1];
			i++;
		} else if (strcmp(argv[i], "subject") == 0) {
			subject = argv[i + 1];
			i++;
		} else {
			printf("<body>Wrong key!</body>\n");
			printf("</html>\n");
			return -1;
		}
	}
	
	for (int i = 0; i < 9; i++) {
		if (strcmp(name, StudentList[i].name) == 0) {
			if (strcmp(subject, "math") == 0) {
				mark = StudentList[i].math_mark;
			} else if (strcmp(subject, "phys") == 0) {
				mark = StudentList[i].phys_mark;
			} else {
				printf("<body>Wrong subject!</body>\n");
				printf("</html>\n");
				return -1;
			}
		}
	}
	if (mark >= 0) {
		printf("<body>%s's %s mark is %d<body>\n", name, subject, mark);
	} else {
		printf("<body>Wrong name!</body>\n");
	}
	printf("<html>\n");
	return 0;
}
