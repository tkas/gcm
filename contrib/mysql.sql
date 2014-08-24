-- phpMyAdmin SQL Dump
-- version 4.2.5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Aug 24, 2014 at 07:13 PM
-- Server version: 5.5.38-0+wheezy1
-- PHP Version: 5.4.4-14+deb7u14

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `gcm-all`
--

-- --------------------------------------------------------

--
-- Table structure for table `attributes`
--

CREATE TABLE IF NOT EXISTS `attributes` (
`id` int(11) NOT NULL,
  `ref_cache` int(11) DEFAULT NULL,
  `attribute_id` int(11) DEFAULT NULL,
  `positive` int(11) DEFAULT NULL
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=424137 ;

-- --------------------------------------------------------

--
-- Table structure for table `fieldnotes`
--

CREATE TABLE IF NOT EXISTS `fieldnotes` (
`id` int(11) NOT NULL,
  `ref_cache` int(11) DEFAULT NULL,
  `date` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `text` text
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `geocaches`
--

CREATE TABLE IF NOT EXISTS `geocaches` (
  `id` int(11) NOT NULL,
  `gcid` text NOT NULL,
  `status` int(11) DEFAULT NULL,
  `has_corrected` int(11) DEFAULT NULL,
  `is_pmo` int(11) DEFAULT NULL,
  `is_own` int(11) DEFAULT NULL,
  `name` text NOT NULL,
  `placed_by` text,
  `owner` text,
  `owner_id` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `size` int(11) DEFAULT NULL,
  `difficulty` int(11) DEFAULT NULL,
  `terrain` int(11) DEFAULT NULL,
  `country` text,
  `state` text,
  `favorite_points` int(11) DEFAULT NULL,
  `dtcreated` int(11) DEFAULT NULL,
  `dtupdate` int(11) DEFAULT NULL,
  `dtfound` int(11) DEFAULT NULL,
  `comment` text,
  `locked` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `geocache_long_data`
--

CREATE TABLE IF NOT EXISTS `geocache_long_data` (
  `id` int(11) NOT NULL,
  `short_desc` text,
  `long_desc` text,
  `short_desc_html` int(11) DEFAULT NULL,
  `long_desc_html` int(11) DEFAULT NULL,
  `hint` text
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `logs`
--

CREATE TABLE IF NOT EXISTS `logs` (
  `id` int(11) NOT NULL,
  `ref_cache` int(11) DEFAULT NULL,
  `date` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `user` text,
  `user_id` int(11) DEFAULT NULL,
  `text` text,
  `isencoded` int(11) DEFAULT NULL,
  `dtupdate` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `points`
--

CREATE TABLE IF NOT EXISTS `points` (
`id` int(11) NOT NULL,
  `latE6` int(11) DEFAULT NULL,
  `lonE6` int(11) DEFAULT NULL,
  `name` text NOT NULL,
  `wpttype` int(11) NOT NULL,
  `subtype` int(11) DEFAULT NULL,
  `ref_cache` int(11) DEFAULT NULL,
  `ref_waypoint` int(11) DEFAULT NULL
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=104195 ;

-- --------------------------------------------------------

--
-- Table structure for table `points_idx_node`
--

CREATE TABLE IF NOT EXISTS `points_idx_node` (
  `nodeno` int(11) NOT NULL,
  `data` blob
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `points_idx_parent`
--

CREATE TABLE IF NOT EXISTS `points_idx_parent` (
  `nodeno` int(11) NOT NULL,
  `parentnode` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `points_idx_rowid`
--

CREATE TABLE IF NOT EXISTS `points_idx_rowid` (
  `rowid` int(11) NOT NULL,
  `nodeno` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `tags`
--

CREATE TABLE IF NOT EXISTS `tags` (
`id` int(11) NOT NULL,
  `ref_cache` int(11) DEFAULT NULL,
  `name` text,
  `value` text
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `waypoints`
--

CREATE TABLE IF NOT EXISTS `waypoints` (
`id` int(11) NOT NULL,
  `ref_cache` int(11) DEFAULT NULL,
  `code` text,
  `name` text,
  `type` int(11) DEFAULT NULL,
  `description` text,
  `user_comment` text,
  `user_flag` int(11) DEFAULT '0',
  `dtupdate` int(11) DEFAULT NULL,
  `corrected` int(11) DEFAULT NULL
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=45121 ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `attributes`
--
ALTER TABLE `attributes`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `fieldnotes`
--
ALTER TABLE `fieldnotes`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `geocaches`
--
ALTER TABLE `geocaches`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `geocache_long_data`
--
ALTER TABLE `geocache_long_data`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `logs`
--
ALTER TABLE `logs`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `points`
--
ALTER TABLE `points`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `points_idx_node`
--
ALTER TABLE `points_idx_node`
 ADD PRIMARY KEY (`nodeno`);

--
-- Indexes for table `points_idx_parent`
--
ALTER TABLE `points_idx_parent`
 ADD PRIMARY KEY (`nodeno`);

--
-- Indexes for table `points_idx_rowid`
--
ALTER TABLE `points_idx_rowid`
 ADD PRIMARY KEY (`rowid`);

--
-- Indexes for table `tags`
--
ALTER TABLE `tags`
 ADD PRIMARY KEY (`id`);

--
-- Indexes for table `waypoints`
--
ALTER TABLE `waypoints`
 ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `attributes`
--
ALTER TABLE `attributes`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=424137;
--
-- AUTO_INCREMENT for table `fieldnotes`
--
ALTER TABLE `fieldnotes`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `points`
--
ALTER TABLE `points`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=104195;
--
-- AUTO_INCREMENT for table `tags`
--
ALTER TABLE `tags`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `waypoints`
--
ALTER TABLE `waypoints`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=45121;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
