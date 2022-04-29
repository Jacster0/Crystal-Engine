#include "Platform/Windows/CrystalWindow.h"
#include "Core/Application.h"
#include "Platform/Windows/Types.h"
#include "Core/Exceptions/CrystalException.h"
#include "Platform/Windows/MessageBox.h"
#include "Core/CLI/Console.h"
#include "Core/Logging/Logger.h"
#include "Core/Logging/ConsoleSink.h"

using namespace Crystal;
int main() {
    try {
        
        Console::Create();

        Logger::AddSink<ConsoleSink>();

         return Application{
             ApplicationCreateInfo{}
         }.Run();
    }
    catch(const CrystalException e) {
        MessageBox::Show(
                e.what(),
                e.GetType(),
                MessageBox::Buttons::OK,
                MessageBox::Icon::Exclamation);

        return 1;
    }
    catch(const std::exception& e) {
        MessageBox::Show(
                e.what(),
                "Standard exception",
                MessageBox::Buttons::OK,
                MessageBox::Icon::Exclamation);

        return 1;
    }

    catch(...) {
        MessageBox::Show(
                "No details available",
                "Unknown exception",
                MessageBox::Buttons::OK,
                MessageBox::Icon::Exclamation);

        return 1;
    }
}