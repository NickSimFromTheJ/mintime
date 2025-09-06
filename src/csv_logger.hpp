#ifndef CSV_LOGGER_H_
#define CSV_LOGGER_H_
#include <fstream>
#include <stdint.h>

class CSVLogger {
    std::ofstream ofs;

  public:
    CSVLogger(const std::string &filename, bool write_header = true) {
        ofs.open(filename, std::ios::out);
        if (write_header) {
            ofs << "op,iters,total_cycles,cycles_per_iter\n";
        }
    }

    void log(const std::string &op, uint64_t iters, uint64_t total_cycles) {
        double cpi =
            static_cast<double>(total_cycles) / static_cast<double>(iters);
        ofs << op << "," << iters << "," << total_cycles << "," << cpi << "\n";
    }

    ~CSVLogger() {
        if (ofs.is_open())
            ofs.close();
    }
};

#endif // CSV_LOGGER_H_
