# Contributing to Direct 5D Viewer

Thank you for your interest in contributing to the Direct 5D Viewer! We welcome contributions to both the C++ rendering engine and the Matlab frontend.

The following is a set of guidelines for contributing to this project.

## Code of Conduct

This project and everyone participating in it is governed by the [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## How Can I Contribute?

### Reporting Bugs

This section guides you through submitting a bug report.
- **Use a clear and descriptive title** for the issue to identify the problem.
- **Describe the exact steps which reproduce the problem** in as much detail as possible.
- **Provide specific examples** to demonstrate the steps.
- **Include screenshots and animated GIFs** which show you following the described steps and clearly demonstrate the problem.
- **Detail your environment**: Matlab version, OS version, GPU model, etc.

### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion, including completely new features and minor improvements to existing functionality.
- **Use a clear and descriptive title** for the issue to identify the suggestion.
- **Provide a step-by-step description of the suggested enhancement** in as much detail as possible.
- **Explain why this enhancement would be useful** to most users.

## Styleguides

### C++ Styleguide

- We follow standard C++11 (and newer) practices.
- Use `PascalCase` for class names and `camelCase` for variables/methods.
- Use `Tabs` for indentation.
- Ensure all resources (DirectX buffers, etc.) are properly released in destructors or via smart pointers.
- New dependencies should be avoided if possible to keep the build simple.

### Matlab/Mex Styleguide

- Matlab functions should use `camelCase`.
- Use `Blocks` for comments explaining complex logic.
- Mex files must rigorously check inputs (`nrhs`, `prhs`) to prevent crashing the Matlab session.

## Pull Requests

1.  Fork the repo and create your branch from `master`.
2.  If you've added code that should be tested, add tests.
3.  Ensure your code builds on Windows (Visual Studio 2017+).
4.  Make sure your code follows the existing style.
5.  Issue that pull request!

## License

By contributing, you agree that your contributions will be licensed under its [BSD 3-Clause License](LICENSE).
