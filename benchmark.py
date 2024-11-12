import subprocess
import math
import json
import argparse
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

def run_simulation(command: str) -> list[bytes]:
    """
    Run the ns-3 simulation with the given command and return the output.

    Args:
        command (str): The command to run the ns-3 simulation.

    Returns:
        list: A list of output lines from the simulation.
    """
    output = subprocess.check_output(
        ["../ns3", "run", command], shell=False, stderr=subprocess.STDOUT
    )
    return output.splitlines()


def save_benchmarks(filename: str, benchmarks: dict[int, float]) -> None:
    """
    Save benchmark results to a JSON file.

    Args:
        filename (str): The name of the file to save the benchmarks.
        benchmarks (dict): A dictionary containing benchmark data.
    """
    with open(filename, "w") as f:
        json.dump(benchmarks, f)


def plot_benchmarks(filename: str) -> None:
    """
    Plot benchmark results from JSON file.

    Args:
        filename (str): The file to plot.
    """
    plot_labels = {
        "nodes-benchmark.json": {
            "xlabel": "Number of nodes",
            "ylabel": "Fails",
            "title": "Failures over nodes (100 packets)",
        },
        "speed-benchmark.json": {
            "xlabel": "Speed of the node (m/s)",
            "ylabel": "Fails",
            "title": "Failures over speed (100 packets)",
        },
        "time-benchmark.json": {
            "xlabel": "Number of nodes",
            "ylabel": "Time (seconds)",
            "title": "Packet delivery time",
        },
    }
    
    with open(filename) as f:
        benchmark = json.loads(f.read())

    x = [int(i) for i in benchmark.keys()]
    y = [benchmark[i] for i in benchmark.keys()]

    plt.plot(x, y, marker="o")

    labels = plot_labels.get(filename, {})
    plt.xlabel(labels.get("xlabel", ""))
    plt.ylabel(labels.get("ylabel", ""))
    plt.title(labels.get("title", ""))

    plt.savefig(filename.replace(".json", ".png"))
    plt.show()


def run_benchmark(
    command_template: str,
    parameter_values: list[int],
    output_index: int,
    filename: str,
    benchmark_name: str,
    plot: bool = False,
) -> None:
    """
    Run a benchmark by executing a simulation multiple times and averaging the results.

    Args:
        command_template (str): The template for the simulation command with a placeholder for the parameter.
        parameter_values (list): A list of parameter values to iterate over.
        output_index (int): The index of the output line to extract the result from.
        filename (str): The name of the file to save the benchmark results.
        benchmark_name (str): The name of the benchmark being run.
        plot (bool): Whether to plot the results after running the benchmark.
    """
    benchmarks = {}

    for value in parameter_values:
        command = command_template.format(value=value)
        results = []

        while len(results) < 10:
            output = run_simulation(command)
            try:
                result_value = float(output[output_index].split(b": ")[1])
                if filename == "time-benchmark.json" and result_value == 0.0:
                    continue

                results.append(result_value)
                print(f"{benchmark_name}: {value}, Result: {result_value}")

            except Exception:
                continue

        average_result = sum(results) / len(results)
        benchmarks[value] = average_result
        print(f"{benchmark_name}: {value}, Avg result: {average_result}")
        save_benchmarks(filename, benchmarks)

    if plot:
        plot_benchmarks(filename)


def benchmark_time(plot: bool = False) -> None:
    """
    Benchmark the simulation time for different numbers of nodes.
    """
    command_template = (
        "lra-simulator --benchmark --duration=1500 --packets=1 --nodes={value}"
    )
    parameter_values = [2 ** i for i in range(1, 12)]
    run_benchmark(
        command_template,
        parameter_values,
        output_index=5,
        filename="time-benchmark.json",
        benchmark_name="Nodes",
        plot=plot,
    )


def benchmark_failure_rate_speed(plot: bool = False) -> None:
    """
    Benchmark the failure rate for different speeds.
    """
    command_template = "lra-simulator --range=30 --nodes=30 --speed={value}"
    parameter_values = [2**i for i in range(0, 12)]
    run_benchmark(
        command_template,
        parameter_values,
        output_index=-1,
        filename="speed-benchmark.json",
        benchmark_name="Speed",
        plot=plot,
    )


def benchmark_failure_rate_nodes(plot: bool = False) -> None:
    """
    Benchmark the failure rate for different numbers of nodes.
    """
    command_template = "lra-simulator --range=30 --speed=1.5 --nodes={value}"
    parameter_values = [2**i for i in range(1, 12)]
    run_benchmark(
        command_template,
        parameter_values,
        output_index=-1,
        filename="nodes-benchmark.json",
        benchmark_name="Nodes",
        plot=plot,
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run benchmarks for lra-simulator.")
    parser.add_argument(
        "benchmark",
        choices=["time", "failure_rate_speed", "failure_rate_nodes"],
        help="Select the benchmark to run.",
    )
    parser.add_argument(
        "--plot",
        action="store_true",
        help="Plot the results after running the benchmark.",
    )
    args = parser.parse_args()

    if args.benchmark == "time":
        benchmark_time(plot=args.plot)
    elif args.benchmark == "failure_rate_speed":
        benchmark_failure_rate_speed(plot=args.plot)
    elif args.benchmark == "failure_rate_nodes":
        benchmark_failure_rate_nodes(plot=args.plot)
