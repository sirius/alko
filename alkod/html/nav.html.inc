<!--
SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
SPDX-License-Identifier: Apache-2.0
-->
      <!-- Static navbar -->
      <div class="navbar navbar-default" role="navigation">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="/"><span class="glyphicon glyphicon-home"><span/> Alko</a>
        </div>
        <div class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li><a href="/log.html"><span class="glyphicon glyphicon-list"><span/> Log</a></li>
            <li><a href="/rawlog.html"><span class="glyphicon glyphicon-list-alt"><span/> Raw log</a></li>
            <li><a href="/check_status.html"><span class="glyphicon glyphicon-info-sign"><span/> Status</a></li>
            <li><a href="/admin.html"><span class="glyphicon glyphicon-wrench"><span/> Admin</a></li>
          </ul>
          <ul class="nav navbar-nav navbar-right">
            <li><a href="#"><span class="glyphicon glyphicon-time"><span/> <b>{$DATETIME}</b></a></li>
            <li><a href="//logout:logout@{$HOST}"><span class="glyphicon glyphicon-eject"><span/> Log out</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
