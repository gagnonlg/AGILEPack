// #include "include/architecture.hh"
// #include "include/layer.hh"
// #include "include/autoencoder.hh"
#include "agile_base.hh"
#include <fstream>
// #include "numeric_handler.hh"

int main(int argc, char const *argv[])
{

    agile::matrix X(4, 2);

    X << 0.0, 0.0,
         0.0, 1.0,
         1.0, 0.0, 
         1.0, 1.0;

    agile::matrix T(4, 1);
    T << 0.0,
         1.0,
         1.0,
         0.0;


    architecture arch; 


    // auto aut1 = autoencoder(2, 4, sigmoid); // can set parms this way
    // auto aut = autoencoder(2, 4, sigmoid); // can set parms this way

    // arch.add_layer(layer_factory<autoencoder>(2, 4, sigmoid));

    // arch.add_a_layer<autoencoder>(2, 4, sigmoid);
    arch.emplace_back(new autoencoder(2, 4, sigmoid));
    arch.emplace_back(new autoencoder(4, 3, sigmoid));
    arch.emplace_back(new layer(3, 1, sigmoid));

    for (int i = 0; i < 100000; ++i)
    {
        for (int point = 0; point < 4; ++point)
        {
            arch.encode(X.row(point), 0, false);
            // aut.encode(X.row(point), false);
        }
    }
    // for (int i = 0; i < 1000; ++i)
    // {
    //     for (int point = 0; point < 4; ++point)
    //     {
    //         arch.encode(X.row(point), 1, false);
    //         // aut.encode(X.row(point), false);
    //     }
    // }
    for (int point = 0; point < 4; ++point)
    {
        // arch.at(0)->encode(X.row(point), true);
        // aut.encode(X.row(point), true);
        std::cout << "input:\n" <<  X.row(point) << "\n";
        std::cout << "reconstructed:\n" << arch.at(0)->reconstruct(X.row(point)) << "\n";
        // std::cout << "reconstructed:\n" << aut.reconstruct(X.row(point)) << "\n";
        getchar();
    }
    YAML::Emitter out;

    // out << YAML::Key << "autoencoder" << YAML::Value << *(arch.at(0)); // save it

    // arch.add_layer(aut);

    // arch.add_layer(2, 4, sigmoid);

    
    



    // arch.add_layer(4, 3, sigmoid);

    // arch += new layer(3, 1, rectified);
    // arch.add_layer(3, 1, rectified);



    // architecture arch({2, 4, 3, 1}, classify);


    std::ofstream file("network.yaml");

    // train it
    for (int i = 0; i < 8000; ++i)
    {
        for (int point = 0; point < 4; ++point)
        {
            arch.correct(X.row(point), T.row(point));
        }
    }

    YAML::Node net;
    net["network"] = arch;
    out << net;
    file << out.c_str();
    file.close();


    //see what it predicts
    std::cout << "Original: " << std::endl;

    for (int i = 0; i < 4; ++i)
    {
        std::cout << "input: " << X.row(i) << ", output: \n" << arch.predict(X.row(i)) << "\n, truth: \n" << T.row(i) << std::endl;
    }



    // now load it and cross check

    YAML::Node config = YAML::LoadFile("network.yaml");
    
    architecture ARCH = std::move(config["network"].as<architecture>());

    // // // layer l = ARCH.at(6); // make sure this throws an error correctly

    std::cout << "loaded: " << std::endl;

    for (int i = 0; i < 4; ++i)
    {
        std::cout << "input: " << X.row(i) << ", output: \n" << ARCH.predict(X.row(i)) << "\n, truth: \n" << T.row(i) << std::endl;
    }

    // std::cout << "encoded: " << encoded << std::endl;

    return 0;
}
