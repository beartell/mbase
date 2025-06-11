# Contribution

Hello! We would like to thank you for your interest in contributing to the development 
of the MBASE project.

This document will briefly explain how the project is structured, branches, PR formatting, and information in general.

> [!IMPORTANT]
> If you are planning to contribute to the core MBASE SDK C++ library, make sure to read the [modules documentation](https://github.com/Emreerdog/mbase/blob/main/MODULAR.md)

## About Examples

Examples are binary executable applications that are written using the MBASE SDK. Even though they are not considered as modules, they have their own
branches assigned to them.

They are not expected to expose a public API to the user so that the CMake library install rules won't be written for example applications except binary install rules.

The example applications will be located under the `examples` directory, each with its own corresponding development branch.

For instance, an example application named `test` will be found under the `examples/test` branch. All related content, including markdown documentation, will be housed within this directory. A dedicated development branch, `examples/test`, will also be created by the repository maintainers. Pull requests related to the `test` application should be submitted to this branch.

## Branch-based Development and Branches

MBASE project employs a branch-based development in order to make the management much easier and make the project less error-prone.

Contributors shouldn't open a PR to the `main` branch since it will be used as a release branch. Instead, the PRs should be opened to one of the branches below:

- `configuration`: CMake configurations and CI workflows are implemented and updated under this branch.
- `documents`: Program documentations, github markup create/delete/update operations are implemented under this branch. I am updating this document in this branch. 
- `inf/devel`: Inference SDK module implementations.
- `pc/devel`: Program core SDK module implementations.
- `std/devel`: Standard library SDK module implementations.
- `json/devel`: JSON library SDK module implementations.
- `mcp/devel`: MCP library SDK module implementations.
- `examples/benchmark`: Benchmark application implementations.
- `examples/embedding`: Embedding application implementations.
- `examples/retrieval`: Retrieval application implementations.
- `examples/simple-conversation`: Simple conversation application implementations.
- `examples/typo-fixer`: Typo fixer application implementations.
- `examples/openai-server`: Openai server application implementations.

The PRs outside of those branches won't be considered.

If you think that you have an implementation idea and there should be a branch assigned to it, create an issue with the format below:

`Branch Request: <branch_name>`

And under the issue, explain your reasoning so that we may open a development branch.

## Issue Format

> [!IMPORTANT]
> Branch request issues are not subject to issue formatting.

There is no imposed standard on issue formatting just be precise and clear.

Please, unless you think it is worth mentioning, do not mindlessly copy-paste thousands of lines of terminal
output. Examine the output and send accordingly.

## Pull-Request Format

The PRs can be send to either randomly contribute or fixing some issue in the issues list. If your PR is solving
and you should link the issue number.

Here is the format:

```
Issue: { NO ISSUE | <ISSUE_NUMBER> }
Target Branch: <branch>
Importance: { Critical | High | Moderate | Low }

<description_here>
```

### Release Workflow

On release days, all the branches will be merged into the top `devel` branch and CIs and tests will run on that branch.
After the CIs pass, the `devel` branch will be merged into the `main` branch and the merge will be tagged as the new release version.

## Coding Guidelines

TBD
