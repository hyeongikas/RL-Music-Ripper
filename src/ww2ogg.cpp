#include <iostream>
#include <fstream>
#include <cstring>
#include "wwriff.h"
#include "stdint.h"
#include "errors.h"

class ww2ogg_options {
    std::string in_filename;
    std::string out_filename;
    std::string codebooks_filename;
    bool inline_codebooks;
    bool full_setup;
    ForcePacketFormat force_packet_format;

public:
    ww2ogg_options() : in_filename(""), out_filename(""), codebooks_filename("packed_codebooks.bin"),
                       inline_codebooks(false), full_setup(false), force_packet_format(kNoForcePacketFormat) {}

    void parse_args(int argc, char **argv);
    const std::string& get_in_filename() const { return in_filename; }
    const std::string& get_out_filename() const { return out_filename; }
    const std::string& get_codebooks_filename() const { return codebooks_filename; }
    bool get_inline_codebooks() const { return inline_codebooks; }
    bool get_full_setup() const { return full_setup; }
    ForcePacketFormat get_force_packet_format() const { return force_packet_format; }
};

void usage() {
    std::cout << "\nusage: ww2ogg input.wav [-o output.ogg] [--inline-codebooks] [--full-setup]" << std::endl
              << "                        [--mod-packets | --no-mod-packets]" << std::endl
              << "                        [--pcb packed_codebooks.bin]" << std::endl << std::endl;
}

int main(int argc, char **argv) {
    std::cout << "Audiokinetic Wwise RIFF/RIFX Vorbis to Ogg Vorbis converter " VERSION " by hcs" << std::endl << std::endl;

    ww2ogg_options opt;

    try {
        opt.parse_args(argc, argv);
    }
    catch (const Argument_error& ae) {
        std::cout << ae << std::endl;
        usage();
        return 1;
    }
    try {
        std::cout << "Input: " << opt.get_in_filename() << std::endl;
        Wwise_RIFF_Vorbis ww(opt.get_in_filename(), opt.get_codebooks_filename(), opt.get_inline_codebooks(),
                             opt.get_full_setup(), opt.get_force_packet_format());
        ww.print_info();
        std::cout << "Output: " << opt.get_out_filename() << std::endl;
        std::ofstream of(opt.get_out_filename().c_str(), std::ios::binary);
        if (!of) throw File_open_error(opt.get_out_filename());
        ww.generate_ogg(of);
        std::cout << "Done!" << std::endl << std::endl;
    }
    catch (const File_open_error& fe) {
        std::cout << fe << std::endl;
        return 1;
    }
    catch (const Parse_error& pe) {
        std::cout << pe << std::endl;
        return 1;
    }
    return 0;
}

void ww2ogg_options::parse_args(int argc, char **argv) {
    bool set_input = false, set_output = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            if (i + 1 >= argc) {
                throw Argument_error("-o needs an option");
            }
            if (set_output) {
                throw Argument_error("only one output file at a time");
            }
            out_filename = argv[++i];
            set_output = true;
        } else if (!strcmp(argv[i], "--inline-codebooks")) {
            inline_codebooks = true;
        } else if (!strcmp(argv[i], "--full-setup")) {
            full_setup = true;
            inline_codebooks = true;
        } else if (!strcmp(argv[i], "--mod-packets") || !strcmp(argv[i], "--no-mod-packets")) {
            if (force_packet_format != kNoForcePacketFormat) {
                throw Argument_error("only one of --mod-packets or --no-mod-packets is allowed");
            }
            if (!strcmp(argv[i], "--mod-packets")) {
                force_packet_format = kForceModPackets;
            } else {
                force_packet_format = kForceNoModPackets;
            }
        } else if (!strcmp(argv[i], "--pcb")) {
            if (i + 1 >= argc) {
                throw Argument_error("--pcb needs an option");
            }
            codebooks_filename = argv[++i];
        } else {
            if (set_input) {
                throw Argument_error("only one input file at a time");
            }
            in_filename = argv[i];
            set_input = true;
        }
    }
    if (!set_input) {
        throw Argument_error("input name not specified");
    }
    if (!set_output) {
        size_t found = in_filename.find_last_of('.');
        out_filename = in_filename.substr(0, found);
        out_filename.append(".ogg");
        if (out_filename == in_filename) {
            out_filename.append("_conv.ogg");
        }
    }
}
