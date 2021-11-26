#include "py-dac.hpp"
#include <iostream>

#include "wideusb/front/dac-front.hpp"
#include "wideusb-pc/asio-utils.hpp"

#include "pybind11/stl.h"

namespace py = pybind11;

class PyDAC
{
public:
    PyDAC(NetService& net_service, Address local_address, Address remote_address);


    size_t max_buffer_size();

    void play_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, const std::vector<float>& data);
    void play_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, const std::vector<float>& big_array);

    void send_data(const std::vector<float>& data);

    void load_more_continious_data(size_t data_left);
private:
    void run();

    const size_t samples_per_package = 50;

    std::vector<float> m_continious_data;
    size_t m_next_continious_index;
    std::unique_ptr<DACFront> m_dac;
    std::vector<float>* m_data;
};

PyDAC::PyDAC(NetService& net_service, Address local_address, Address remote_address)
{
    Waiter<bool> waiter;
    m_dac.reset(new DACFront(net_service, waiter.get_waiter_callback(), local_address, remote_address));
    bool success = waiter.wait();
    if (!success)
        throw std::runtime_error("DAC module creation failed");
}

void PyDAC::play_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, const std::vector<float>& data)
{
    Waiter<int> waiter;
    m_dac->init_sample(buffer_size, prescaler, period, repeat, waiter.get_waiter_callback());
    int result = waiter.wait();
    if (result != 0)
        throw std::runtime_error("DAC initialization failed with code " + std::to_string(result));
    send_data(data);
    run();
}

void PyDAC::play_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, const std::vector<float>& big_array)
{
    Waiter<int> waiter;
    m_dac->init_continious(buffer_size, prescaler, period, dma_chunk_size, dma_chunk_size / 2, waiter.get_waiter_callback(), [this](size_t data_left){ load_more_continious_data(data_left); });
    int result = waiter.wait();
    if (result != 0)
        throw std::runtime_error("DAC initialization failed with code " + std::to_string(result));
    m_continious_data = big_array;
    m_next_continious_index = 0;
    load_more_continious_data(0);
    run();
}

void PyDAC::send_data(const std::vector<float>& data)
{
    std::cout << "Data send beginned... data size = " << data.size() << std::endl;
    m_dac->send_data(data, nullptr);
    /*Waiter<size_t> waiter;
    m_dac->send_data(data, waiter.get_waiter_callback());
    std::cout << "Waiting for confirmation" << std::endl;
    size_t actual_sent = waiter.wait();
    if (actual_sent != data.size()*2)
        throw std::runtime_error("DAC data send was incomplete: " + std::to_string(actual_sent) + " btes actual sent of " + std::to_string(data.size()*2));*/
    std::cout << "Data send complete" << std::endl;
}

void PyDAC::run()
{
    std::cout << "Calling DAC run..." << std::endl;
    Waiter<bool> waiter;
    m_dac->run(waiter.get_waiter_callback());
    if (!waiter.wait())
        throw std::runtime_error("DAC failed to run");
    std::cout << "DAC run complete" << std::endl;
}

size_t PyDAC::max_buffer_size()
{
    return 1000;
}

void PyDAC::load_more_continious_data(size_t samples_needed)
{
    std::cout << "load_more_continious_data called, data_left = " << samples_needed << "m_next_continious_index = " << m_next_continious_index << std::endl;
    size_t actual_data_size = std::min(samples_per_package, std::min(samples_needed, m_continious_data.size() - m_next_continious_index));
    if (actual_data_size == 0)
    {
        std::cout << "actual_data_size == 0, stopping" << std::endl;
        m_dac->stop(nullptr);
        return;
    }
    /*if (actual_data_size == 0)
    {
        std::cout << "actual_data_size == 0, stopping" << std::endl;
        Waiter<bool> waiter;
        m_dac->stop(waiter.get_waiter_callback());
        if (!waiter.wait())
            throw std::runtime_error("DAC failed to run");
        std::cout << "stopped" << std::endl;
        return;
    }*/
    std::vector<float> data_chunk(m_continious_data.begin() + m_next_continious_index, m_continious_data.begin() + m_next_continious_index + actual_data_size);
    m_next_continious_index += actual_data_size;
    //send_data(data_chunk);
    m_dac->send_data(data_chunk, nullptr);
}

void add_dac(pybind11::module& m)
{
    py::class_<PyDAC>(m, "DAC")
        .def(py::init<NetService&, Address, Address>(),
             py::arg("device"),
             py::arg("local_address"),
             py::arg("remote_address"))
        .def("play_sample", &PyDAC::play_sample, py::arg("buffer_size"), py::arg("prescaler"), py::arg("period"), py::arg("repeat"), py::arg("data"))
        .def("play_continious", &PyDAC::play_continious, py::arg("buffer_size"), py::arg("prescaler"), py::arg("period"), py::arg("dma_chunk_size"), py::arg("data"))
        ;
}


