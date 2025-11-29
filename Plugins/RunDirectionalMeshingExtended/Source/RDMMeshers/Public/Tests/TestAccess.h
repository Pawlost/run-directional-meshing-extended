#pragma once

#if WITH_AUTOMATION_TESTS
    #define PRIVATE_MODIFIER  protected
#else
	#define PRIVATE_MODIFIER  private
#endif