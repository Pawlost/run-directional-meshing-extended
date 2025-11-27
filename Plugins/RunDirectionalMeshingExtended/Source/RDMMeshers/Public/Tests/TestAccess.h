#pragma once

#if WITH_AUTOMATION_TESTS
    #define TEST_PRIVATE_MODIFIER  protected
#else
	#define TEST_PRIVATE_MODIFIER  private
#endif