#include <unistd.h>
#include <iostream>

#include "src/components/text-label.h"

using namespace readline;

int main() {
  TextLabel *textlabel1 = new TextLabel(nullptr);
  textlabel1->SetText("Teste");
  sleep(1);
  textlabel1->SetText("Teste other");
  textlabel1->SetBackgroundColor("\e[42m");
  sleep(1);
  textlabel1->SetText("Teste nice");
  textlabel1->SetBackgroundColor("\e[44m");
  sleep(1);
  textlabel1->SetText("Teste test");
  textlabel1->SetBackgroundColor("\e[46m");

  return 0;
}
