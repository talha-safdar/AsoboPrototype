#include "AsoboPrototype.h"

#include "Core/Application.h"

int main()
{
	Application application;

	if (!application.Initialize())
	{
		return -1;
	}

	application.Run();

	return 0;
}
