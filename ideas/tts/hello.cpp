#include <festival/festival.h>


int main(int argc, char **argv)
{
    EST_Wave wave;
    int heap_size = 210000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded

    festival_initialize(load_init_files, heap_size);

    // Say simple file
/*
    festival_say_file("/etc/motd");
*/

    // Say some text;
    festival_say_text("hello world");

    // Convert to a waveform
    festival_text_to_wave("hello world",wave);
    wave.save("/tmp/wave.wav","riff");

    // festival_say_file puts the system in async mode so we better
    // wait for the spooler to reach the last waveform before exiting
    // This isn't necessary if only festival_say_text is being used (and
    // your own wave playing stuff)
    festival_wait_for_spooler();

    return 0;
}
