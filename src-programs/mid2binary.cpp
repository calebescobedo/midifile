#include "../src-library/styleTransfer.cpp"


void convert_files_to_binary(std::string  file_with_filepaths, std::string  output_file_path);

int main(int argc, char** argv) {


	std::string file_path_dir = "/data/hibbslab/data/midi/file_paths/";
	std::string classical_guitar_filename = "classical_guitar.txt";
	std::string americana_folk_filename = "americana_folk.txt";


	//Load in all folk songs and convert them to binary
	std::string output_binary_dir = "/data/hibbslab/cescobed/midi_input/binary/";
	std::string classical_dir = "trainB/";
	std::string americana_dir = "trainA/";

	std::string test_file_in =
	"/data/hibbslab/data/midi/AMERICANA_FOLK_www.pdmusic.org_MIDIRip/civilwar/cws01.mid";
	std::string test_file_out =
	"/data/hibbslab/cescobed/midi_input/binary/americana_folk/cws01.txt";


	//Load in all songs of some other type
	//smf::MidiFile temp_midi(test_file_in);
	//Binary_Converter myConverter(temp_midi);
	//myConverter.write_to_binary_long_form(test_file_out);

	//convert_files_to_binary(file_path_dir + americana_folk_filename ,output_binary_dir + americana_dir);
	convert_files_to_binary(file_path_dir + classical_guitar_filename, output_binary_dir + classical_dir);

	return 0;
}
void convert_files_to_binary(std::string  file_with_filepaths, std::string  output_file_path){

	std::cout << " In file converter! " << std::endl;

	std::string cur_line;
	int file_num = 0;
	int not_read = 0;

	std::ifstream in_stream(file_with_filepaths);
	if(in_stream.is_open()){
		while(std::getline(in_stream, cur_line)){
			std::cout <<"NR: " << not_read << " FN: " << file_num << " " << cur_line << std::endl;
			smf::MidiFile temp_midi(cur_line);
			Binary_Converter converter(temp_midi);

			auto track = converter.getChannelAsVec(26, 128);
			std::ofstream cur_file;
			cur_file.open(output_file_path + std::to_string(file_num) + ".txt");
			converter.printToFile(track, cur_file);
			cur_file.close();
			file_num++;
		}

	}else{
		not_read++;
	}
}
