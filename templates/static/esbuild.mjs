// SPDX-FileCopyrightText: (C) 2026 Matthias Fehring <https://www.huessenbergnetz.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

import * as esbuild from 'esbuild';
import { sassPlugin } from 'esbuild-sass-plugin';

const nodeEnv = process.env.NODE_ENV || 'development';
const isProduction = nodeEnv === 'production';

await esbuild.build({
    entryPoints: ['assets/js/gikwimi.js'],
    bundle: true,
    write: true,
    outdir: '.',
    outbase: 'assets',
    target: [
        'firefox60'
    ],
    minify: isProduction
});
