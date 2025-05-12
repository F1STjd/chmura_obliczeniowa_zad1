#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/std.h>

#include <httplib.h>

static constexpr auto port = 3000;
struct city
{
  std::string name;
  std::pair<double, double> coords;
};

static const std::map<std::string, std::array<city, 3>> locations = {
  { "Poland",
    { city { .name = "Warsaw", .coords = { 52.2297, 21.0122 } },
      city { .name = "Krakow", .coords = { 50.0647, 19.9450 } },
      city { .name = "Wroclaw", .coords = { 51.1079, 17.0385 } } } },
  { "USA",
    { city { .name = "New York", .coords = { 40.7128, -74.0060 } },
      city { .name = "Los Angeles", .coords = { 34.0522, -118.2437 } },
      city { .name = "Chicago", .coords = { 41.8781, -87.6298 } } } },
  { "UK",
    { city { .name = "London", .coords = { 51.5074, -0.1278 } },
      city { .name = "Manchester", .coords = { 53.4808, -2.2426 } },
      city { .name = "Edinburgh", .coords = { 55.9533, -3.1883 } } } },
  { "Japan",
    { city { .name = "Tokyo", .coords = { 35.6895, 139.6917 } },
      city { .name = "Osaka", .coords = { 34.6937, 135.5023 } },
      city { .name = "Kyoto", .coords = { 35.0116, 135.7681 } } } }
};

auto
parse_weather(std::string_view jsonstr) -> std::pair<std::string, std::string>
{
  const auto temperature_start_pos = jsonstr.rfind("\"temperature\":");
  jsonstr.remove_prefix(temperature_start_pos + 14);
  const auto temperature_end_pos = jsonstr.find(',');
  const auto temperature_str =
    std::string { jsonstr.substr(0, temperature_end_pos) };

  const auto wind_speed = jsonstr.rfind("\"windspeed\":");
  jsonstr.remove_prefix(wind_speed + 12);
  const auto wind_speed_end_pos = jsonstr.find(',');
  const auto wind_speed_str =
    std::string { jsonstr.substr(0, wind_speed_end_pos) };

  return std::make_pair(temperature_str, wind_speed_str);
}

auto
fetch_weather(double latitude, double longitude)
  -> std::pair<std::string, std::string>
{
  std::string url = fmt::format(
    "/v1/forecast?latitude={}&longitude={}&current_weather=true", latitude,
    longitude);

  httplib::Client cli("api.open-meteo.com");
  auto res = cli.Get(url);
  if (!res || res->status != 200) { fmt::println("Failed to fetch weather"); }

  return parse_weather(res->body);
}

auto
main() -> std::int32_t
{
  const auto time = std::chrono::system_clock::now();
  fmt::println("{}\nKonrad Nowak\nport: {}", time, port);

  httplib::Server server;
  server.Get(
    "/",
    [](const auto& req, auto& res) -> void
    {
      auto& params = req.params;
      std::string html = "<h1>Select Location</h1>"
                         "<form method=\"get\" action=\"/\">"
                         "<label>Country: <select name=\"country\" "
                         "onchange=\"this.form.submit()\">"
                         "<option value=\"\">-- Select Country --</option>";

      for (const auto& [ country, cities ] : locations)
      {
        bool is_selected = false;
        if (auto it = params.find("country"); it != params.end())
        {
          is_selected = (it->second == country);
        }

        html += fmt::format(
          "<option value=\"{}\"{}>{}</option>", country,
          is_selected ? " selected" : "", country);
      }

      html += "</select></label><br>"
              "<label>City: <select name=\"city\">"
              "<option value=\"\">-- Select City --</option>";

      auto xd = params.find("country");
      if (xd != params.end())
      {
        if (auto country_it = locations.find(xd->second);
            country_it != locations.end())
        {
          for (const auto& city : country_it->second)
          {
            bool is_selected = false;
            if (auto it = params.find("city"); it != params.end())
            {
              is_selected = (it->second == city.name);
            }

            html += fmt::format(
              "<option value=\"{}\"{}>{}</option>", city.name,
              is_selected ? " selected" : "", city.name);
          }
        }
      }

      html += "</select></label><br>"
              "<button type=\"submit\">Get Weather</button>"
              "</form>";

      auto country_search = params.find("country");
      auto city_search = params.find("city");
      if (country_search != params.end() && city_search != params.end())
      {

        if (auto country_it = locations.find(country_search->second);
            country_it != locations.end())
        {

          if (auto city_it = std::ranges::find_if(
                country_it->second, [ &city_search ](const city& city)
                { return city.name == city_search->second; });
              city_it != country_it->second.end())
          {
            const auto& [ lat, lon ] = city_it->coords;
            const auto [ temperature, wind_speed ] = fetch_weather(lat, lon);
            html += fmt::format(
              "<p>Temperature: {}</p>"
              "<p>Wind Speed: {}</p>",
              temperature, wind_speed);
          }
        }
      }

      res.set_content(html, "text/html");
    });

  server.listen("0.0.0.0", port);
}
