# 🛠️ Confix: Your Friendly Configuration Manager 🚀

Confix is a powerful yet simple CLI tool designed to help you manage package configurations effortlessly. 📦✨

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Contributing](#contributing)
- [License](#license)

## Features

- 🔍 **Check** if packages are installed
- 📥 **Install** missing packages
- 📝 **Manage** configurations with ease
- 🌟 Supports **JSON** and **JSONC** (kindoff)

Get your configurations fixed with Confix! 🛠️🔧

## Prerequisites

Before you begin, ensure you have the following prerequisites installed on your system:

- **Arch-based Linux distribution**: Confix is specifically designed for Arch-based distributions.
- **yay**: Confix utilizes `yay` as the package manager for installing and managing packages.
- **JSON configuration file**: You need a valid JSON configuration file located at `~/.config/confix/packages.json` that lists the packages you want Confix to manage.

## Dependencies

Confix requires the following dependencies to be installed on your system:

- `jsoncpp`: A C++ library for handling JSON data.
- `argparse`: A command-line argument parsing library.

You can install these dependencies on Arch-based systems using the following command:

```bash
sudo pacman -S --noconfirm jsoncpp argparse
```

## Installation

1. Clone this project to your local machine:

```bash
git clone https://github.com/Srinath10X/Confix.git
```

2. Change directory to the project folder:

```bash
cd Confix
```

3. Compile the source code using the provided Makefile:

```bash
make
```

4. Install Confix to your local bin directory:

```bash
make install
```

5. Ensure your local bin directory is in your PATH (if not already):

```bash
export PATH=$PATH:$HOME/.local/bin:$HOME/bin
```

## Usage

After installation, you can run Confix from the terminal:

```bash
confix
```

Confix will read the configuration file (`~/.config/confix/packages.json` or `~/.config/confix/packages.jsonc`), check for the specified packages, and install any that are missing.

## Configuration

Ensure your configuration file (`~/.config/confix/packages.json` or `~/.config/confix/packages.jsoc`) follows this structure:

```json
{
	"packages": [
		"package1",
		"package2",
		"package3",
		"package4",
		"....",
		"packageN"
	]
}
```

List all the packages you want Confix to manage under the `"packages"` array.

## Contributing

Contributions are welcome! If you have suggestions or improvements, please feel free to open an issue or a pull request.

## License

This project is licensed under the MIT License.
