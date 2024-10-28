#pragma once

namespace statsimproex {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::BindingSource^  Form1BindingSource;
	private: Microsoft::Reporting::WinForms::ReportViewer^  reportViewer1;

	private: System::ComponentModel::IContainer^  components;
	protected: 

	protected: 

	protected: 


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->reportViewer1 = (gcnew Microsoft::Reporting::WinForms::ReportViewer());
			this->Form1BindingSource = (gcnew System::Windows::Forms::BindingSource(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Form1BindingSource))->BeginInit();
			this->SuspendLayout();
			// 
			// reportViewer1
			// 
			this->reportViewer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->reportViewer1->LocalReport->ReportEmbeddedResource = L"Report1.rdlc";
			//this->reportViewer1->LocalReport->ReportPath = L"X:\\documents\\statsim\\current StatSimPro\\project\\statsimproex\\statsimproex\\Report1" 
			//	L".rdlc";
			this->reportViewer1->LocalReport->ReportPath = L"D:\\Documents\\StatSim\\current StatSimPro\\project\\statsimproex\\statsimproex\\Report1.rdlc";
			this->reportViewer1->Location = System::Drawing::Point(0, 0);
			this->reportViewer1->Name = L"reportViewer1";
			this->reportViewer1->Size = System::Drawing::Size(919, 321);
			this->reportViewer1->TabIndex = 0;
			this->reportViewer1->Load += gcnew System::EventHandler(this, &Form1::reportViewer1_Load_1);
			// 
			// Form1BindingSource
			// 
			this->Form1BindingSource->DataSource = statsimproex::Form1::typeid;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(919, 321);
			this->Controls->Add(this->reportViewer1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Form1BindingSource))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				 this->reportViewer1->RefreshReport();
				 this->reportViewer1->RefreshReport();
				 this->reportViewer1->RefreshReport();
			 }
	private: System::Void reportViewer1_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void reportViewer1_Load_1(System::Object^  sender, System::EventArgs^  e) {
			 }
	};
}

