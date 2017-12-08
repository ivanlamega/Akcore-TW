namespace DataEditor {

	using namespace System;
	using namespace System::Text;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class ListData
	{
	private:
		int _Value;
		String^ _Text;
	public:
		property uint32_t uValue
		{
			uint32_t get()
			{
				return _Value;
			}

			void set(uint32_t x)
			{
				_Value = x;
			}

		}
		property int Value
		{
			int get()
			{
				return _Value;
			}

			void set(int x)
			{
				_Value = x;
			}
		}
		property String^  Text
		{
			String^ get()
			{
				return _Text;
			}

			void set(String^ x)
			{
				_Text = x;
			}
		}
	};
}