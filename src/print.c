// Small unit to deal with the formatting of words etc

static const char* yn[] = {"\x1b[32mINP\x1b[0m", "OUT"};
static const char* vals[] = {"0", "\x1b[91m1\x1b[0m"};

void printHeader()
{
	printf("+------------------------------------------------+\n");
  printf("| PIN  | MEM ADDR  | GPIO NAME | STATE  | VALUE  |\n"); 
	printf("+------------------------------------------------+\n");
}

void printPin(Pin *pin)
{
	int gpio = memToGpioLabel(pin->memIndex);
  printf("| %02d   |    %02d     | ", 
    pin->chipIndex, pin->memIndex);
	if (gpio >= 0) printf("Gpio %2d   ", gpio);
	else printf("          ");
	printf("| %s    |  %sV    |\n",
		yn[pin->state], vals[pin->value]);
}

void printAll()
{
	printHeader();
	for (int i = 0; i < NO_OF_PINS; i++)
		printPin(chip->pins[i]);
	printf("+------------------------------------------------+\n");
}
