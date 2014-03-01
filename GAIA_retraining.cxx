#include "Base"
#include "include/parser.hh"



template <class T, class U>
std::ostream& operator << ( std::ostream& o, const std::map<T, U> &m)
{
    for (auto &e : m)
    {
        o << e.first << ":\n" << e.second << std::endl;
    }
    return o;
}


void complain(const std::string &complaint);

//----------------------------------------------------------------------------
int main(int argc, char const *argv[])
{

    optionparser::parser p("A CLI for retraining GAIA.");
//----------------------------------------------------------------------------
    p.add_option("--file", "-f")    .help("Pass at least one file to add to a TChain for testing.")
                                    .mode(optionparser::store_mult_values);
    //----------------------------------------------------------------------------
    p.add_option("--tree", "-t")    .help("Name of the TTree to extract.")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("--load")          .help("Name of a YAML neural network file to load.")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("--config", "-c")  .help("Branch config file")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("--learning")      .help("Pass a learning rate.")
                                    .mode(optionparser::store_value)
                                    .default_value(0.1);
    //----------------------------------------------------------------------------
    p.add_option("--momentum")      .help("Pass a momentum value.")
                                    .mode(optionparser::store_value)
                                    .default_value(0.5);
    //----------------------------------------------------------------------------
    p.add_option("--regularize")    .help("Pass an l2 norm regularizer.")
                                    .mode(optionparser::store_value)
                                    .default_value(0.00001);
    p.add_option("--batch")         .help("Mini-batch size.")
                                    .mode(optionparser::store_value)
                                    .default_value(10);
    //----------------------------------------------------------------------------
    p.add_option("--load")          .help("Name of a YAML neural network file to load to begin training")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("--config", "-c")  .help("pass a config file")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("--struct")        .help("pass the network structure")
                                    .mode(optionparser::store_mult_values);
    //----------------------------------------------------------------------------
    p.add_option("--deepauto", "-d").help("how many autoencoders to train?")
                                    .mode(optionparser::store_value)
                                    .default_value(-1);
    //----------------------------------------------------------------------------
    p.add_option("--type", "-T")    .help("Can be one of 'regress', 'multiclass', or 'binary'.")
                                    .mode(optionparser::store_value)
                                    .default_value("regress");
    //----------------------------------------------------------------------------
    p.add_option("--verbose", "-v") .help("Make the output verbose");
    //----------------------------------------------------------------------------
    p.add_option("--weights", "-w") .help("print a file with the first layer weight matrix.")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("-start")          .help("Start index for training. (Default = 0)")
                                    .mode(optionparser::store_value)
                                    .default_value(0);
    //----------------------------------------------------------------------------
    p.add_option("-end")            .help("End index for training. (Default, whole tree)")
                                    .mode(optionparser::store_value)
                                    .default_value(-1);  
    //----------------------------------------------------------------------------
    p.add_option("-uepochs")        .help("Number of passes over the Trees for Unsupervised Pretraining(Default = 5)")
                                    .mode(optionparser::store_value)
                                    .default_value(5);
    //----------------------------------------------------------------------------
    p.add_option("-sepochs")        .help("Number of passes over the Trees for Supervised Training(Default = 10)")
                                    .mode(optionparser::store_value)
                                    .default_value(10);
    //----------------------------------------------------------------------------
    p.add_option("--formula", "-F") .help("Specify Model Formula")
                                    .mode(optionparser::store_value);
    //----------------------------------------------------------------------------
    p.add_option("-start")          .help("Start index for testing/training. (Default = 0)")
                                    .mode(optionparser::store_value)
                                    .default_value(0);
    //----------------------------------------------------------------------------
    p.add_option("-end")            .help("End index for testing/training. (Default, whole tree)")
                                    .mode(optionparser::store_value)
                                    .default_value(-1);
    //----------------------------------------------------------------------------
    p.add_option("--predict")       .help("Prediction mode!");
//----------------------------------------------------------------------------

    p.eat_arguments(argc, argv);

    if (!p.get_value("file")) complain("need to pass at least one file.");

    if (!p.get_value("tree")) complain("need to pass a tree name.");

    if (!p.get_value("load") && p.get_value("predict")) complain("need a neural network to load in prediction mode.");

    if (!p.get_value("config")) complain("need a root branch config file.");


    std::vector<std::string> root_files(p.get_value<std::vector<std::string>>("file"));

    std::string ttree_name =    p.get_value<std::string>("tree"),
                load_file =     p.get_value<std::string>("load"),
                config_file =   p.get_value<std::string>("config");

    int     start =       p.get_value<int>("start"),
            end =         p.get_value<int>("end");

    // bool    verbose =     p.get_value("verbose");

//----------------------------------------------------------------------------

    agile::root::tree_reader tree_buf;

    for (auto &file : root_files) tree_buf.add_file(file, ttree_name);

    tree_buf.set_branches(config_file);

    agile::model_frame frame;

    frame.add_data(std::move(
        tree_buf.get_dataframe(start, end - start, p.get_value("verbose"))));

    




//----------------------------------------------------------------------------
    agile::neural_net net;

    net.from_yaml(load_file);

    std::vector<std::string> input_vars {"pt", "bottom", "charm", "light", "MV1"};


    std::cout << "prob_bottom, bottom, charm, light, pt, MV1" << std::endl;
    for (int i = start; i < end; ++i)
    {
        auto pred = net.predict_map(TR(i, net.get_inputs()));

        auto control = TR(i, input_vars);

        std::cout << pred["bottom"] << "," << control["bottom"] << "," << control["charm"] << "," << control["light"] << "," << control["pt"] << "," << control["MV1"] << std::endl;
    }



    return 0;
}


void complain(const std::string &complaint)
{
    std::cerr << "Error: " << complaint << std::endl;
    exit(1);
}