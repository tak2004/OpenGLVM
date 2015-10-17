/*
 * Copyright (c) 2013, Nick Gravelyn.
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 * 
 */

using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.Serialization;

namespace GLCSGen
{
    public class GLSpec
    {
        [XmlElement("Comment")]
        public string HeaderComment;

        [XmlArrayItem("Version")]
        public List<GLVersion> Versions = new List<GLVersion>();

        public static GLSpec FromFile(string xmlFile)
        {
            XDocument doc = XDocument.Load(xmlFile);
            GLSpec spec = new GLSpec();
            spec.HeaderComment = doc.Root.Element("comment").Value;

            foreach (var e in doc.Root.Elements("feature"))
            {
                // OpenGL is super special because it simply includes all enums and commands
                if (e.Attribute("api").Value == "gl" && e.Attribute("number").Value == "1.0")
                {
                    AddVersion1(spec, e);
                }
                // OpenGL 3.0 and up have Compatibility profile and Core profile
                else if (e.Attribute("api").Value == "gl" && int.Parse(e.Attribute("number").Value[0].ToString()) >= 3)
                {
                    AddVersion(spec, e, "Compatibility");
                    AddVersion(spec, e, "Core");
                }
                // OpenGL pre-3.0 and OpenGL ES don't have profiles
                else
                {
                    AddVersion(spec, e, string.Empty);
                }
            }

            return spec;
        }

        /// <summary>
        /// Super special OpenGL 1 version handler
        /// </summary>
        private static void AddVersion1(GLSpec spec, XElement versionElem)
        {
            var version = new GLVersion
            {
                Api = "GL",
                Number = "1.0",
                Profile = string.Empty,
                Name = "GL10"
            };

            // First we process commands like any other version
            foreach (var require in versionElem.Elements("require"))
            {
                foreach (var commandElem in require.Elements("command"))
                {
                    var command = new GLCommand { Name = commandElem.Attribute("name").Value };
                    command.Initialize(commandElem.Document);
                    version.Commands.Add(command);
                }
            }

            // Then we go through all included commands and use all the groups on any parameters
            // to include all the enums we need from the groups section
            foreach (var group in (from c in version.Commands from p in c.Parameters select p.Group).Distinct())
            {
                if (string.IsNullOrEmpty(group))
                {
                    continue;
                }

                foreach (var enumElem in versionElem.Document.Root.Element("groups").Elements("group").Where(e => e.Attribute("name").Value == group).Elements("enum"))
                {
                    if (!version.Enums.ContainsKey(enumElem.Attribute("name").Value))
                    {
                        string enumName = enumElem.Attribute("name").Value;
                        version.Enums.Add(enumName, GetEnumValue(versionElem, enumName));
                    }
                }
            }

            spec.Versions.Add(version);
        }

        /// <summary>
        /// Creates a given version.
        /// </summary>
        private static void AddVersion(GLSpec spec, XElement versionElem, string profile)
        {
            var version = new GLVersion
            {
                Api = versionElem.Attribute("api").Value.ToUpper(),
                Number = versionElem.Attribute("number").Value,
                Profile = profile
            };

            if (version.Api != "GL")
            {
                version.Api = "GLES";
            }

            version.Name = version.Api + version.Number.Replace(".", "");
            if (!string.IsNullOrEmpty(profile))
            {
                version.Name += "" + profile;
            }

            // Add all enums and commands from previous version
            for (int i = spec.Versions.Count - 1; i >= 0; i--)
            {
                var previousVersion = spec.Versions[i];
                if (previousVersion.Api == version.Api &&
                    (previousVersion.Profile == string.Empty || previousVersion.Profile == version.Profile))
                {
                    foreach (var pair in previousVersion.Enums)
                    {
                        version.Enums.Add(pair.Key, pair.Value);
                    }

                    foreach (var command in previousVersion.Commands)
                    {
                        version.Commands.Add(command.Clone());
                    }

                    // only handle previous one version of same API and profile since it will have folded
                    // all enums/commands in turn
                    break;
                }
            }

            // Include all new enums and commands
            foreach (var require in versionElem.Elements("require"))
            {
                foreach (var enumElem in require.Elements("enum"))
                {
                    if (!version.Enums.ContainsKey(enumElem.Attribute("name").Value))
                    {
                        string enumName = enumElem.Attribute("name").Value;
                        version.Enums.Add(enumName, GetEnumValue(versionElem, enumName));
                    }
                }

                foreach (var commandElem in require.Elements("command"))
                {
                    var command = new GLCommand { Name = commandElem.Attribute("name").Value };
                    if (version.Commands.Find(c => c.Name == command.Name) != null)
                    {
                        continue;
                    }

                    command.Initialize(commandElem.Document);
                    version.Commands.Add(command);
                }
            }

            // Remove any enums and commands
            foreach (var remove in versionElem.Elements("remove"))
            {
                if (remove.Attribute("profile") == null || 
                    remove.Attribute("profile").Value.ToLower() == profile.ToLower())
                {
                    foreach (var en in remove.Elements("enum"))
                    {
                        version.Enums.Remove(en.Attribute("name").Value);
                    }
                    foreach (var en in remove.Elements("command"))
                    {
                        version.Commands.RemoveAll(c => c.Name == en.Attribute("name").Value);
                    }
                }
            }

            spec.Versions.Add(version);
        }

        private static string GetEnumValue(XElement versionElem, string enumName)
        {
            string enumValue = string.Empty;

            foreach (var enumsElements in versionElem.Document.Root.Elements("enums"))
            {
                foreach (var enumElement in enumsElements.Elements("enum"))
                {
                    if (enumElement.Attribute("name").Value == enumName &&
                        (enumElement.Attribute("api") == null || enumElement.Attribute("api").Value == versionElem.Attribute("api").Value))
                    {
                        enumValue = enumElement.Attribute("value").Value;
                        break;
                    }
                }

                if (!string.IsNullOrEmpty(enumValue))
                {
                    break;
                }
            }
            return enumValue;
        }
    }
    
    public class GLVersion
    {
        [XmlAttribute]
        public string Api;

        [XmlAttribute]
        public string Name;

        [XmlAttribute]
        public string Profile;

        [XmlAttribute]
        public string Number;

        public EnumDictionary Enums = new EnumDictionary();

        [XmlArrayItem("Command")]
        public List<GLCommand> Commands = new List<GLCommand>();
    }

    public class GLParameter
    {
        [XmlAttribute]
        public string Group;

        [XmlAttribute]
        public string Type;

        [XmlAttribute]
        public string Name;
    }

    public class GLCommand
    {
        [XmlAttribute]
        public string Name;

        [XmlAttribute]
        public string ReturnType;

        [XmlArrayItem("Parameter")]
        public List<GLParameter> Parameters = new List<GLParameter>();

        public GLCommand Clone()
        {
            return new GLCommand
            {
                Name = Name,
                ReturnType = ReturnType,
                Parameters = (from p in Parameters select new GLParameter { Name = p.Name, Type = p.Type, Group = p.Group }).ToList()
            };
        }

        public void Initialize(XDocument specDoc)
        {
            var commandElem = (from elem in specDoc.Root.Element("commands").Elements("command")
                               where elem.Element("proto").Element("name").Value == Name
                               select elem).First();

            ReturnType = commandElem.Element("proto").Value.Replace(Name, string.Empty).Trim();

            foreach (var p in commandElem.Elements("param"))
            {
                var param = new GLParameter { Name = p.Element("name").Value };
                param.Type = p.Value.Substring(0, p.Value.LastIndexOf(param.Name)).Trim();
                if (p.Attribute("group") != null)
                {
                    param.Group = p.Attribute("group").Value;
                }
                Parameters.Add(param);
            }
        }
    }

    public class EnumDictionary : Dictionary<string, string>, IXmlSerializable
    {
        System.Xml.Schema.XmlSchema IXmlSerializable.GetSchema()
        {
            return null;
        }

        void IXmlSerializable.ReadXml(System.Xml.XmlReader reader)
        {
            // we don't care about parsing our format back
        }

        void IXmlSerializable.WriteXml(System.Xml.XmlWriter writer)
        {
            foreach (var entry in this)
            {
                writer.WriteStartElement("Enum");
                writer.WriteAttributeString("Name", entry.Key);
                writer.WriteAttributeString("Value", entry.Value);
                writer.WriteEndElement();
            }
        }
    }
}
