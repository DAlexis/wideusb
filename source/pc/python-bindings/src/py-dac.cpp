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

    void play_sample(uint32_t prescaler, uint32_t period, bool repeat, const std::vector<float>& data);
    void play_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period, const std::vector<float>& data);
    void stop();

private:
    void run();
    void send_data_by_chunks(const float* data, size_t size, bool sync);
    size_t load_more_continious_data(size_t data_left);

    const size_t samples_per_package = 100;

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

void PyDAC::play_sample(uint32_t prescaler, uint32_t period, bool repeat, const std::vector<float>& data)
{
    Waiter<int> waiter;
    m_dac->init_sample(data.size(), prescaler, period, repeat, waiter.get_waiter_callback());
    int result = waiter.wait();
    if (result != 0)
        throw std::runtime_error("DAC initialization failed with code " + std::to_string(result));
    send_data_by_chunks(data.data(), data.size(), true);
    run();
}

void PyDAC::play_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period, const std::vector<float>& data)
{
    Waiter<int> waiter;
    m_dac->init_continious(dma_chunk_size, prescaler, period, waiter.get_waiter_callback(), [this](size_t data_left){ load_more_continious_data(data_left); });
    int result = waiter.wait();
    if (result != 0)
        throw std::runtime_error("DAC initialization failed with code " + std::to_string(result));
    m_continious_data = data;
    send_data_by_chunks(m_continious_data.data(), dma_chunk_size, true);
    m_next_continious_index = dma_chunk_size;
    run();
}

void PyDAC::stop()
{
    Waiter<bool> waiter;
    m_dac->stop(waiter.get_waiter_callback());
    if (!waiter.wait())
    {
        throw std::runtime_error("DAC stop caused an error");
    }
}

void PyDAC::send_data_by_chunks(const float* data, size_t size, bool sync)
{
    //std::cout << "Data send beginned... data size = " << data.size() << std::endl;
    for (size_t begin = 0; begin != size; )
    {
        size_t block_size = std::min(size - begin, samples_per_package);
        if (sync)
        {
            Waiter<bool> waiter;
            m_dac->send_data(&data[begin], block_size, waiter.get_waiter_callback());
            waiter.wait();
        } else {
            m_dac->send_data(&data[begin], block_size, nullptr);
        }
        begin += block_size;
    }
    //std::cout << "Data send complete" << std::endl;
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

size_t PyDAC::load_more_continious_data(size_t samples_needed)
{
    std::cout << "load_more_continious_data called, data_left = " << samples_needed << "m_next_continious_index = " << m_next_continious_index << std::endl;
    size_t actual_data_size = std::min(samples_per_package, std::min(samples_needed, m_continious_data.size() - m_next_continious_index));
    if (actual_data_size == 0)
    {
        std::cout << "actual_data_size == 0, stopping" << std::endl;
        m_dac->stop(nullptr);
        return 0;
    }
    m_dac->send_data(&m_continious_data[m_next_continious_index], actual_data_size, nullptr);
    std::vector<float> data_chunk(m_continious_data.begin() + m_next_continious_index, m_continious_data.begin() + m_next_continious_index + actual_data_size);
    m_next_continious_index += actual_data_size;
    return actual_data_size;
}

void add_dac(pybind11::module& m)
{
    py::class_<PyDAC>(m, "DAC")
        .def(py::init<NetService&, Address, Address>(),
             py::arg("device"),
             py::arg("local_address"),
             py::arg("remote_address"))
        .def("play_sample", &PyDAC::play_sample, py::arg("prescaler"), py::arg("period"), py::arg("repeat"), py::arg("data"))
        .def("play_continious", &PyDAC::play_continious, py::arg("dma_chunk_size"), py::arg("prescaler"), py::arg("period"), py::arg("data"))
        .def("stop", &PyDAC::stop)
        ;
}


