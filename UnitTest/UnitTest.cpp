#include "pch.h"
#include "CppUnitTest.h"
#include "../UDPdispatcher/framework.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// I wasn't sure if std::queue copied or what so I tested it
			// and you just get what you provided so beware
			struct thing {
				~thing(){ delete[] data; }
				char* data{};
			};
			char* c = _strdup("Hello World");
			SafeQueue<thing> t;
			{
				thing f1;
				thing f2;
				f2.data = c;
				t.enqueue(f1);
				t.enqueue(f2);
			}
			{
				thing f3 = t.dequeue();
				thing f4 = t.dequeue();
				Assert::IsTrue(f3.data==nullptr);
				Assert::IsTrue(f4.data==c);				// it has simple copied not deep copied 
			}
		}
	};
}
