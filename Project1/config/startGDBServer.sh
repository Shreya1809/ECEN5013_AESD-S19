#!/bin/bash

PORT=5500

gdbserver --multi localhost:$PORT
